CC=/usr/bin/gcc
FLAGS=-Wall -g
TARGET=uvgTorrent
PATHS=src/debug
default:
	make clean
	make build
	make test

clean:
	rm -f bin/$(TARGET)

build:
	$(CC) -I$(PATHS) -std=c11 -Wall -g -o bin/$(TARGET) src/uvgTorrent.c -lm

test:
	valgrind --leak-check=full ./bin/$(TARGET) 11
