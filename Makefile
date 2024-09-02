include config.mk

# not nmake \
!ifdef 0
.DEFAULT_GOAL:=fmake
# \
!endif

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

test:
	@./fmake ENABLE_DEBUG=1

.PHONY: test clean
