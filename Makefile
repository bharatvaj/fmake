include config.mk

SOURCE = fmake.c config.h

fmake.exe: $(SOURCE)
	cl /nologo $(cl_CFLAGS) $(CFLAGS) fmake.c

fmake: $(SOURCE)
	$(CC) -o $@ $(_CFLAGS) $(CFLAGS) $(LDFLAGS) fmake.c

clean:
	-rm fmake
	-del fmake.exe

install: fmake
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f fmake $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/fmake

test:
	@./fmake ENABLE_DEBUG=1

.PHONY: test clean
