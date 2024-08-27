VERSION = 0.1.6

$(CC)_CFLAGS = -DFMAKE_VERSION=\"$(VERSION)\" -Wall -Wextra -g
cl_CFLAGS = /DFMAKE_VERSION=\"$(VERSION)\" /Zi

PREFIX = /usr/local
