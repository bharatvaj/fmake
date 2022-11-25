#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "config.h"

void process_build() {
	printf("%s %s", maker.cmd, maker.args);
}

struct stat st = {0};
void process_string() {
	for (int i = 0; i < (sizeof(makers) / sizeof(maker_config_t)); i++) {
		const char* filename = makers[i].filename;
		if (!stat(filename, &st)) {
			maker = makers[i];
			process_build();
			return;
		}
	}
	maker = makers[FMAKE_POSIX_MAKEFILE];
	process_build();
}


int main(int argc, char* argv[]) {
	process_string();
	return -1;
}
