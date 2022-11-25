#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "config.h"

// Two things to do
// scan directory and pick out the maker
// process the input string and convert it for the `maker`
// output the full commands

char* process_string() {
	char* s = (char*)malloc(256);
	strcpy(s, "hello, wold");
	struct stat st = {0};
	for (int i = 0; i < sizeof(filenames); i++) {
		if (!stat(filenames[i], &st)) {
			printf("File detected %s", filenames[i]);
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
