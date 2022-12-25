include config.mk

all: fmake

fmake: fmake.c

clean:
	rm -f fmake

install: fmake
	mkdir -p $(DESTDIR)$(PREFIX)/bin 
	cp -f fmake $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/fmake

test:
	@./fmake ENABLE_DEBUG=1

.PHONY: all test clean
