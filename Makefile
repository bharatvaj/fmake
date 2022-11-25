include config.mk

PROGRAM = fmake

SRC = $(PROGRAM).c

all: $(PROGRAM)

$(PROGRAM): $(SRC) config.h

clean:
	rm $(PROGRAM)

test:
	@./$(PROGRAM) ENABLE_DEBUG=1

.PHONY: all test clean
