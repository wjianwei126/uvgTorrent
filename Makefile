CC=/usr/bin/gcc
FLAGS=-Wall -g -D_POSIX_SOURCE -std=c11 -D_BSD_SOURCE 
TARGET=uvgTorrent 
INCLUDE=-Isrc
OBJECTS=

default:
	make clean
	make build
	make test

clean:
	rm -f bin/$(TARGET)

build:
	$(CC) $(INCLUDE) $(OBJECTS) $(FLAGS) -o bin/$(TARGET) $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c)

test:
	valgrind --leak-check=full --track-origins=yes ./bin/$(TARGET) torrents/test.magnet
