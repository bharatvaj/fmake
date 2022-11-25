include config.mk

PROGRAM = fmake

SRC = $(PROGRAM).c

all: $(PROGRAM)

$(PROGRAM): $(SRC) config.h

clean:
	rm $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) $(DESTDIR)/$(PREFIX)

test:
	@./$(PROGRAM) ENABLE_DEBUG=1

.PHONY: all test clean
