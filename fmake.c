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

typedef enum {
	FMAKE_FIL = 0,
	FMAKE_DIR = 1,
	FMAKE_EXT = 2
} filetype_t;

typedef struct {
	short level;
	filetype_t filetype;
	const char* filename;
	const char* cmd;
	const char* args[256];
	size_t args_len;
} maker_config_t;

#include "config.h"

static maker_config_t maker;

static short should_execute_commands = 1;
static short is_accepting_cmd_args = 0;
static short is_verbose = 0;

#ifdef _WIN32
static HANDLE dir;
static WIN32_FIND_DATA entry;
#else
static DIR *dir;
static struct dirent *entry;
#endif

#define info(...) \
	if (should_execute_commands) { \
		fprintf(stderr, __VA_ARGS__); \
	}

int
process_build(int argc, char* argv[]) {
	int status = -1;
	info("++");
	for(size_t bs_i = 0; bs_i < maker.args_len && maker.args[bs_i] != NULL; bs_i++) {
		fprintf(should_execute_commands? stderr : stdout, " %s", maker.args[bs_i]);
	}
	info("\n");
	if (should_execute_commands) {
		fflush(stderr);
		if (*maker.args[0] == '\0') {
			status = execlp(maker.cmd, maker.cmd, '\0', (void*)0);
		}
		else {
			status = execvp(maker.cmd, (char* const*)maker.args);
		}
		if (status == -1) {
			perror(maker.cmd);
		}
	}
	return status;
}

inline int
is_file_present(short filetype, const char* name, size_t name_len) {
	switch(filetype) {
#ifdef _WIN32
		case FMAKE_FIL:
		case FMAKE_EXT:
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
		case FMAKE_EXT:
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
		case FMAKE_DIR:
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

void
fmake_usage(int status) {
	printf("Usage: fmake [options] [target] ...\n");
	printf("Options:\n");
	char* opt_array[] = {
		"-?", "Prints fmake usage",
		"-D", "Print various types of debugging information.",
		"-N", "Don't actually run any build commands; just print them.",
		"-V", "Print the version number of make and exit."
	};
	for(int i = 0; i < sizeof(opt_array) / sizeof(char**); i += 2) {
		printf("  %-27s %s\n", opt_array[i], opt_array[i + 1]);
	}
	exit(status);
}

int
main(int argc, char* argv[]) {
	int bs_i = 0;
	int makers_len = sizeof(makers) / sizeof(maker_config_t);
	for(bs_i = 1; bs_i < argc; bs_i++) {
		if (!(argv[bs_i][0] == '-' && argv[bs_i][1] != '\0')) {
			exit(-1);
		}
		switch(argv[bs_i][1]) {
			case '-':
				is_accepting_cmd_args = 1;
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
#ifndef _WIN32
	dir = opendir("./");
	if (dir == NULL) {
		perror("fmake");
		return -1;
	}
#endif
	for (bs_i = 0; bs_i < makers_len; bs_i++) {
		const char *str = makers[bs_i].filename;
		if (is_file_present(makers[bs_i].filetype, str, sizeof(makers[bs_i].filename)) == 0) {
			break;
		} else if (is_verbose) {
			fprintf(stderr, "fmake: %s (%d)\n", makers[bs_i].filename, makers[bs_i].filetype);
		}
	}
	maker = makers[bs_i == makers_len? 0 : bs_i];
	return process_build(argc, argv);
}
