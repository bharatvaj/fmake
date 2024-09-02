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
	FM_NOP = 0,  /* no-op */
	FM_CBB = 1,  /* chdir before building */
	FM_CAB = 2,  /* chdir after building */
	FM_MKD = 4,  /* mkdir */
} operation_t;

typedef struct maker_config {
	short level;
	filetype_t filetype;
	operation_t operation;
	const char *filename;
	const char *cmd;
	const char *args[32];
	size_t args_len;
	const struct maker_config *chain;
} maker_config_t;

#include "config.h"

static const size_t makers_len = sizeof(makers) / sizeof(maker_config_t);
static maker_config_t maker;

static short
	should_execute_commands = 1, /* if 0 commands will be printed to stdout */
	level_requested = 1,         /* -1 indicates no level requested */
	is_verbose = 0;
	bs_i = 0;

char *errormsg;
size_t errormsg_len;

#define info(...) \
	if (is_verbose) { \
		fprintf(stderr, "fmake: " __VA_ARGS__); \
	}

#define errprint(...) \
		fprintf(stderr, "fmake: " __VA_ARGS__);

inline int
is_file_present(short filetype, const char* name, size_t name_len) {
	switch(filetype) {
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
					char* dot = strrchr(entry->d_name, '.');
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
	if (dir) closedir(dir);
#endif
	return 0;
}

int
check_files(int maker_i) {
	do {
		maker = makers[maker_i];
		if (is_file_present(maker.filetype,
				maker.filename, strlen(maker.filename)) == 0)
			break;

		info("%s\n", maker.filename);
	} while(++maker_i < makers_len || makers[maker_i].level <= level_requested);

	if (maker_i == makers_len) maker_i = 0;
	return maker_i;
}

int
process_build(int argc, char* argv[]) {
	int status = -1;
	FILE* fd = should_execute_commands? stderr : stdout;
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	int tmpint;
	size_t cmdargs_len = 0;

	if (should_execute_commands && (maker.operation & FM_MKD)) {
		errprint("Running mkdir for 'out'\n");
		if (CreateDirectory("out", NULL) == 0) {
			switch(GetLastError()) {
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

	if (should_execute_commands) fprintf(fd, "++ ");
	for(size_t args_i = 0; args_i < maker.args_len && maker.args[args_i] != NULL; args_i++) {
		tmpint = strlen(maker.args[args_i]);
		/* +1 for space */
		cmdargs_len += tmpint + 1;
		fprintf(fd, "%.*s ", tmpint, maker.args[args_i]);
	}
	if (should_execute_commands) fprintf(fd, "\n");
	fflush(fd);

	if (!should_execute_commands) return 0;

#ifdef _WIN32
	char *cmdargs = (char *)malloc(cmdargs_len);
	tmpint = 0;

	/* Windows can't accept argv[] style args. This converts it to a string */
	for(int i = 0; i < maker.args_len && maker.args[i] != NULL; i++) {
		size_t cmdargs_pointer = (size_t) maker.args[i];
		strcpy(&cmdargs[tmpint], maker.args[i]);
		tmpint += strlen(maker.args[i]);
		cmdargs[tmpint++] = ' ';
	}
	cmdargs[cmdargs_len] = '\0';

	if (!CreateProcess( NULL, cmdargs, NULL, NULL, FALSE, 0, NULL, NULL,
				&si, &pi)) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			errprint("%s: not found\n", maker.cmd);
		}
		return -1;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	if (GetExitCodeProcess(pi.hProcess, &status) == 0) {
		errprint("fatal: cannot get process status for %d",  pi.dwProcessId);
		return -1;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

#else
	if (*maker.args[0] == '\0') {
		status = execlp(maker.cmd, maker.cmd, '\0', (void*)0);
	}
	else {
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
		maker = maker.chain? *maker.chain :
			makers[check_files(maker.level - 1)];
		return process_build(argc, argv);
	}
	return 0;
}

void
fmake_usage(int status) {
	printf("Usage: fmake [options] [target] ...\n");
	printf("Options:\n");
	char* opt_array[] = {
		"-?",       "Prints fmake usage",
		"-C path",  "Change to directory before calling the programs",
		"-D",       "Print various types of debugging information.",
		"-N",       "Don't actually run any build commands; just print them.",
		"-V",       "Print the version number of make and exit."
	};
	for(int i = 0; i < sizeof(opt_array) / sizeof(char**); i += 2) {
		printf("  %-27s %s\n", opt_array[i], opt_array[i + 1]);
	}
	exit(status);
}

int
main(int argc, char* argv[]) {
	char working_dir[256];
	for(bs_i = 1; bs_i < argc; bs_i++) {
		if (!(argv[bs_i][0] == '-' && argv[bs_i][1] != '\0')) {
			exit(-1);
		}
		switch(argv[bs_i][1]) {
			case '-':
				goto FMAKE_AFTER_ARG_CHECK;
				break;
			case 'l':
				// TODO show better listing
				/* list supported build systems */
				for (size_t bs_i = 0; bs_i < makers_len; bs_i++) {
					maker = makers[bs_i];
					printf("%-5d %-5d %-25s %-20s", maker.level, maker.filetype, maker.filename, maker.cmd);
					for(size_t i = 1; i < maker.args_len; i++) printf("%s ", maker.args[i]);
					printf("\n");
				}
				break;
			case 'C':
				if (++bs_i == argc) {
					fprintf(stderr, "fmake.exe: Option requires an argument\n");
					fmake_usage(-1);
				}
				if (chdir(argv[bs_i]) == -1){
					errprint("fmake: *** %s: %s.  Stop.\n", argv[bs_i], strerror(errno));
					return -1;
				}
				break;
			case '1':
			case '2':
			case '3':
				level_requested = atoi(&argv[bs_i][1]);
				break;
			case 'N':
				should_execute_commands = 0;
				break;
			case 'V':
				printf("fmake " FMAKE_VERSION);
				return 0;
				break;
			case 'D':
				is_verbose=1;
				break;
			case '?':
				fmake_usage(0);
				break;
		}
	}

FMAKE_AFTER_ARG_CHECK:
	argc = argc - bs_i;
	argv = argv + bs_i;
	bs_i = -1;

#ifndef _WIN32
	dir = opendir("./");
	if (dir == NULL) {
		perror("fmake");
		return -1;
	}
#endif

	/* foward through levels if requested */
	while(makers[++bs_i].level < level_requested);

	maker = makers[check_files(bs_i)];

	return process_build(argc, argv);
}
