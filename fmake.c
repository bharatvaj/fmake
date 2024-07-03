#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "config.h"

static maker_config_t maker;
static DIR *dir;
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
			printf("Error: %d\n", status);
		}
	}
	return status;
}


int main(int argc, char* argv[]) {
	int i;
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
	for (size_t i = 0; i < (sizeof(makers) / sizeof(maker_config_t)); i++) {
		if (makers[i].check_extension) {
            // TODO optimize this code
            // FIXME it's not sure if the '.' is found from first or last
            // last should be preferred
            dir = opendir("./");
            if (dir == NULL) {
                perror("fmake:");
                return -1;
            }
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_REG) {
                    char* dot = strrchr(entry->d_name, '.');
                    if (dot && strcmp(dot + 1, makers[i].filename) == 0) {
                        maker = makers[i];
                        return process_build(argv);
                    }
                }
            }
			printf("extension check %s\n", makers[i].filename);
		} else {
			const char* filename = makers[i].filename;
			if (access(filename, F_OK) == 0) {
				maker = makers[i];
				return process_build(argv);
			}
		}
	}
	maker = makers[0];
	return process_build(argv);
}
