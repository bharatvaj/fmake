SRC = fatmake.c

all: fatmake

fatmake: $(SRC)

clean:
	rm fatmake

test:
	./fatmake ENABLE_DEBUG=1
