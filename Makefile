CC=/usr/bin/gcc
FLAGS=-Wall -g
TARGET=uvgTorrent
INCLUDE=-Isrc/debug

default:
	make clean
	make build
	make test

clean:
	rm -f bin/$(TARGET)

build:
	$(CC) $(INCLUDE) -std=c11 -Wall -g -o bin/$(TARGET) src/uvgTorrent.c -lm

test:
	valgrind --leak-check=full ./bin/$(TARGET) torrents/test.torrent
