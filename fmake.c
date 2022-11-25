#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "config.h"

void process_build() {
	printf("%s %s", maker.cmd, maker.args);
}

void process_string() {
	struct stat st = {0};
	short maker_found = 0;
	for (int i = 0; i < sizeof(makers); i++) {
		const char* filename = makers[i].filename;
		if (!stat(filename, &st)) {
			maker = makers[i];
			maker_found = 1;
			process_build();
			break;
		}
	}
	if (maker_found == 0) {
		maker = makers[FMAKE_POSIX_MAKEFILE];
		process_build();
	}
}


int main(int argc, char* argv[]) {
	process_string();
	return -1;
}
