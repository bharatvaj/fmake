/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

#ifdef _WIN32
static HANDLE dir;
static WIN32_FIND_DATA entry;
#else
static DIR *dir;
static struct dirent *entry;
#endif

typedef enum {
	FM_FIL = 1,
	FM_DIR = 2,
	FM_EXT = 4,
} filetype_t;

typedef enum {
	FM_NOP = 0,					/* no-op */
	FM_CBB = 1,					/* chdir before building */
	FM_CAB = 2,					/* chdir after building */
	FM_MKD = 4,					/* mkdir */
} operation_t;

typedef struct maker_config {
	short level;
	filetype_t filetype;
	operation_t operation;
	const char *filename;
	const char *cmd;
	const char *args[32];
	int args_len;
	const struct maker_config *chain;
	const char **argmap;
	int argmap_len;
} maker_config_t;

#include "config.h"

static const size_t makers_len = sizeof(makers) / sizeof(maker_config_t);
static maker_config_t maker;

// TODO FIX Possible CVE
char *input_toks[256];
int input_toks_i[256];
size_t input_toks_len = 0;

char *targets[256];
size_t targets_len = 0;

static short should_execute_commands = 1;	/* if 0 commands will be printed to stdout */
static short pref_level_requested = 1;	/* -1 indicates no level requested */
static short pref_is_verbose = 0;

char *errormsg;
size_t errormsg_len;

#define info(...) \
	if (pref_is_verbose) { \
		fprintf(stderr, "fmake: " __VA_ARGS__); \
	}

#define errprint(...) \
		fprintf(stderr, "fmake: " __VA_ARGS__);

inline int
is_file_present(short filetype, const char *name, size_t name_len)
{
	switch (filetype) {
#ifdef _WIN32
	case FM_FIL:
	case FM_EXT:
		dir = FindFirstFile(name, &entry);
		if (dir == INVALID_HANDLE_VALUE) {
			return -1;
		}
		goto FMAKE_FOUND_MATCH;
		break;
#else
	case FMAKE_FIL:
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {
				if (strcmp(entry->d_name, name) == 0) {
					goto FMAKE_FOUND_MATCH;
				}
			}
		}
		break;
	case FM_EXT:
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {
				char *dot = strrchr(entry->d_name, '.');
				if (dot && strcmp(dot + 1, name) == 0) {
					goto FMAKE_FOUND_MATCH;
				}
			}
		}
		break;
#endif
	case FM_DIR:
		if (access(name, 0) == 0) {
			goto FMAKE_FOUND_MATCH;
		}
		break;
	}
	return -1;
FMAKE_FOUND_MATCH:
#ifndef _WIN32
	if (dir)
		closedir(dir);
#endif
	return 0;
}

char *
get_val(char **arr, size_t arr_len, char *key)
{
	for (int i = 0; i < arr_len; i += 2) {
		if (strcmp(key, arr[i]) == 0)
			return arr[i + 1];
	}
	return NULL;
}

int
check_files(int maker_i)
{
	do {
		maker = makers[maker_i];
		if (is_file_present(maker.filetype,
							maker.filename, strlen(maker.filename)) == 0)
			break;

		info("%s\n", maker.filename);
	} while (++maker_i < makers_len
			 || makers[maker_i].level <= pref_level_requested);

	if (maker_i == makers_len)
		maker_i = 0;
	return maker_i;
}

int
process_build(int argc, char *argv[])
{
	int status = -1;
	FILE *fd = should_execute_commands ? stderr : stdout;
#ifdef _WIN32
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
#endif
	int tmpint;
	size_t cmdargs_len = 0;

	if (should_execute_commands && (maker.operation & FM_MKD)) {
		errprint("Running mkdir for 'out'\n");
		if (CreateDirectory("out", NULL) == 0) {
			switch (GetLastError()) {
			case ERROR_ALREADY_EXISTS:
				break;
			case ERROR_PATH_NOT_FOUND:
				errprint("Cannot create 'out' directory\n");
			default:
				return -1;
			}
		}
	}

	if (maker.operation & FM_CBB) {
		errprint("Entering directory 'out'\n");
		if (chdir("out") != 0) {
			perror("fmake");
			return -1;
		}
	}
	// FIXME Make copies only if input_token is given
	/* Transform arg values of type=release => /Configuration:Release etc */
	char **args = (char **) malloc(maker.args_len + input_toks_len);
	int args_len = maker.args_len;

	for (int i = 0; i < args_len && maker.args[i] != NULL; i++) {
		size_t len_s = strlen(maker.args[i]) + 1;
		args[i] = (char *) malloc(len_s);
		strncpy(args[i], maker.args[i], len_s);
	}
	/* maker.cmd == "cmake" */
	/*
	 * if there is value in input_toks, then try to convert it
	 * if not possible, use the original values
	 */
	char *newval;
	for (int i = 0; i < input_toks_len && input_toks[i] != NULL; i++) {
		/* errprint("%d/%zd: %s\n", i, input_toks_len, input_toks[i]); */
		if (maker.argmap_len) {
			/* errprint("input_toks[%d]= %s\n", i, input_toks[i]); */
			if (newval =
				get_val(maker.argmap, maker.argmap_len, input_toks[i])) {
				/* set the string 'type' => /configuration: to args[args_len] */
				args[args_len] = newval;
				if (input_toks[i + 1]
					&& (newval =
						get_val(maker.argmap, maker.argmap_len,
								input_toks[i + 1]))) {
					/* append value part to args[args_len] */
					args[args_len] = strcat(args[args_len], newval);
				} else {
					// append debug values
				}
				args_len++;
				continue;
			}
		}
		/* errprint("uhh: argv[input_toks_i[%d]] = %s\n", i, argv[input_toks_i[i]]); */
		/* just append the values from argv */
		args[args_len++] = argv[input_toks_i[i]];
	}

	/* for(int i = 0; i < args_len; i++) { */
	/*  fprintf(stderr, "%s ", args[i]); */
	/* } */
	/* fprintf(stderr, "\n"); */


#ifndef _WIN32
	if (should_execute_commands)
		fprintf(fd, "++ ");
	for (size_t args_i = 0; args_i < args_len && args[args_i] != NULL;
		 args_i++) {
		tmpint = strlen(args[args_i]);
		/* +1 for space */
		cmdargs_len += tmpint + 1;
		fprintf(fd, "%.*s ", tmpint, args[args_i]);
	}
	if (should_execute_commands)
		fprintf(fd, "\n");
	fflush(fd);

	if (!should_execute_commands)
		return 0;
#endif

#ifdef _WIN32
	char *cmdargs = (char *) malloc(BUFFER_ALLOC_SZ);
	size_t cmdargs_cap = BUFFER_ALLOC_SZ;
	cmdargs_len = 0;

	/* Windows can't accept argv[] style args. This converts it to a string */
	for (int i = 0; i < args_len && args[i] != NULL; i++) {
		size_t arglen = strlen(args[i]);
		// we append an extra space for argument, so we check for the extra 1 byte availability
		if ((cmdargs_len + arglen + 1) > cmdargs_cap) {
			cmdargs_cap = cmdargs_len + arglen + BUFFER_ALLOC_SZ;
			cmdargs = realloc(cmdargs, cmdargs_cap);
		}
		strncpy(cmdargs + cmdargs_len, args[i], arglen);
		cmdargs_len += arglen;
		// just append an extra ' ' space to avoid excessive checking
		cmdargs[cmdargs_len++] = ' ';
	}

	if (cmdargs_len > 0)
		cmdargs[cmdargs_len--] = '\0';

	if (should_execute_commands)
		fprintf(fd, "++ ");


	// TODO FIX this will most definitely get trimmed
	fprintf(fd, "%.*s\n", (int) cmdargs_len, cmdargs);
	fflush(fd);

	if (!should_execute_commands)
		return 0;

	if (!CreateProcess(NULL, cmdargs, NULL, NULL, FALSE, 0, NULL, NULL,
					   &si, &pi)) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			errprint("%s: not found\n", maker.cmd);
		}
		return -1;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	if (GetExitCodeProcess(pi.hProcess, &status) == 0) {
		errprint("fatal: cannot get process status for %d",
				 pi.dwProcessId);
		return -1;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

#else
	if (*maker.args[0] == '\0') {
		status = execlp(maker.cmd, maker.cmd, '\0', (void *) 0);
	} else {
		status = execvp(maker.cmd, maker.args);
	}
#endif

	if (status != 0) {
		info("'%s' exited with non-zero exit code\n", maker.cmd);
		return status;
	}

	if (maker.operation & FM_CAB) {
		errprint("Entering directory 'out'\n");
		if (chdir("out") != 0) {
			perror("fmake");
			return -1;
		}
	}

	if (maker.level > 1) {
		maker = maker.chain ? *maker.chain :
			makers[check_files(maker.level - 1)];
		return process_build(argc, argv);
	}
	return 0;
}

void
fmake_usage(int status)
{
	printf("Usage: fmake [options] [target] ...\n");
	printf("Options:\n");
	char *opt_array[] = {
		"-?", "Prints fmake usage",
		"-C path", "Change to directory before calling the programs",
		"-D", "Print various types of debugging information.",
		"-N", "Don't actually run any build commands; just print them.",
		"-V", "Print the version number of make and exit."
	};
	for (int i = 0; i < sizeof(opt_array) / sizeof(char **); i += 2) {
		printf("  %-27s %s\n", opt_array[i], opt_array[i + 1]);
	}
	exit(status);
}

int
main(int argc, char *argv[])
{
	int argi = 0;
	for (argi = 1; argi < argc; argi++) {
		if ((argv[argi][0] == '-' && argv[argi][1] != '\0')) {
			switch (argv[argi][1]) {
			case '-':
				goto FMAKE_AFTER_ARG_CHECK;
				break;
			case 'l':
				// TODO show better listing
				/* list supported build systems */
				for (size_t argi = 0; argi < makers_len; argi++) {
					maker = makers[argi];
					printf("%-5d %-5d %-25s %-20s", maker.level,
						   maker.filetype, maker.filename, maker.cmd);
					for (size_t i = 1; i < maker.args_len; i++)
						printf("%s ", maker.args[i]);
					printf("\n");
				}
				break;
			case 'C':
				if (++argi == argc) {
					fprintf(stderr,
							"fmake.exe: Option requires an argument\n");
					fmake_usage(-1);
				}
				if (chdir(argv[argi]) == -1) {
					errprint("fmake: *** %s: %s.  Stop.\n", argv[argi],
							 strerror(errno));
					return -1;
				}
				break;
			case '1':
			case '2':
			case '3':
				pref_level_requested = atoi(&argv[argi][1]);
				break;
			case 'N':
				should_execute_commands = 0;
				break;
			case 'V':
				printf("fmake " FMAKE_VERSION);
				return 0;
				break;
			case 'D':
				pref_is_verbose = 1;
				break;
			case '?':
				fmake_usage(0);
				break;
			}
		} else if (input_toks[input_toks_len] = strtok(argv[argi], "=")) {
			input_toks_i[input_toks_len++] = argi;
			input_toks[input_toks_len] = strtok(NULL, "=");
		} else {
			targets[targets_len++] = argv[argi];
		}
	}

FMAKE_AFTER_ARG_CHECK:
	argi = -1;
#ifndef _WIN32
	dir = opendir("./");
	if (dir == NULL) {
		perror("fmake");
		return -1;
	}
#endif

	/* foward through levels if requested */
	while (makers[++argi].level < pref_level_requested);

	maker = makers[check_files(argi)];

	return process_build(argc, argv);
}
