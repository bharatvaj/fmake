include config.mk

SOURCE = fmake.c config.h

fmake.exe: $(SOURCE)
	cl /nologo $(CFLAGS) fmake.c

fmake: $(SOURCE)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) fmake.c

clean:
	-$(RM) fmake fmake.exe fmake.i fmake.pdb fmake.ilk fmake.obj

install: fmake
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f fmake $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/fmake

format:
	$(EXPORT) VERSION_CONTROL=none
	indent -kr -ppi 0 -il 0 -psl --use-tabs -ts4 -br -brs -ce -cli0 *.c *.h
	dos2unix *.c *.h

test:
	@./fmake ENABLE_DEBUG=1

.PHONY: test clean
