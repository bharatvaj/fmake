#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "config.h"

static maker_config_t maker;
// TODO assigning NULL can be avoided
static DIR *dir = NULL;
static struct dirent *entry;
static short should_execute_commands = 0;
static short is_accepting_cmd_args = 0;


#define info(...) \
	if (should_execute_commands) { \
		fprintf(stderr, __VA_ARGS__); \
	}

int process_build(char* argv[]) {
	int status = -1;
	info("++");
	for(size_t i = 0; i < sizeof(maker.args) && maker.args[i] != NULL; i++) {
		fprintf(should_execute_commands? stderr : stdout , " %s", maker.args[i]);
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
			//printf("Error: %d\n", status);
		}
	}
	return status;
}


int main(int argc, char* argv[]) {
	int i = 0;
	int makers_size = sizeof(makers) / sizeof(maker_config_t);
	for(i = 1; i < argc; i++) {
		if (!(argv[i][0] == '-' && argv[i][1] != '\0')) {
			printf("Usage: fmake [-l]\n");
			exit(-1);
		}
		switch(argv[i][1]) {
			case '-':
				is_accepting_cmd_args = 1;
				goto FMAKE_AFTER_ARG_CHECK;
				break;
			case 'l':
				// TODO show better listing
				/* list supported build systems */
				for (size_t i = 0; i < (sizeof(makers) / sizeof(maker_config_t)); i++) {
					printf("%s\n", makers[i].cmd);
				}
				break;
			case 'a':
				should_execute_commands = 1;
				break;
		}
	}
FMAKE_AFTER_ARG_CHECK:
	argc = i;
	argv = argv + i;
	for (i = 0; i < makers_size; i++) {
		if (makers[i].file_type == FMAKE_EXT) {
			// TODO optimize this code
			// FIXME it's not sure if the '.' is found from first or last
			// last should be preferred
			dir = opendir("./");
			if (dir == NULL) {
				perror("fmake");
				return -1;
			}
			while ((entry = readdir(dir)) != NULL) {
				if (entry->d_type == DT_REG) {
					char* dot = strrchr(entry->d_name, '.');
					if (dot && strcmp(dot + 1, makers[i].filename) == 0) {
						goto FMAKE_FOUND_MATCH;
					}
				}
			}
		} else {
			const char* filename = makers[i].filename;
			if (access(filename, F_OK) == 0) {
				goto FMAKE_FOUND_MATCH;
			}
		}
	}
FMAKE_FOUND_MATCH:
	if (dir) closedir(dir);
	maker = makers[i == makers_size? 0 : i];
	return process_build(argv);
}
