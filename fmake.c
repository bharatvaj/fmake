#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "config.h"

// Two things to do
// scan directory and pick out the maker
// process the input string and convert it for the `maker`
// output the full commands

void process_build() {
	printf("%s", maker.cmd);
}

char* process_string() {
	char* s = (char*)malloc(256);
	struct stat st = {0};
	for (int i = 0; i < sizeof(makers); i++) {
		const char* filename = makers[i].filename;
		if (!stat(filename, &st)) {
			maker = makers[i];
			process_build();
			break;
		}
	}
	return s;
}

// support -- arguments for cmake and other stuff
// fmake -- --preset x86-64-apple-darwin

int main(int argc, char* argv[]) {
	printf("%s\n", process_string());
	return -1;
}
