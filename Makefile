VERSION = 0.1.3

CFLAGS += -DFMAKE_VERSION=$(VERSION) -Wall -Wextra -g
PREFIX   ?= /usr/local

all: fmake

fmake: fmake.c config.h

clean:
	rm -f fmake

install: fmake
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f fmake $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/fmake

test:
	@./fmake ENABLE_DEBUG=1

.PHONY: all test clean
