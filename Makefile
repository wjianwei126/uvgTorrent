CC=/usr/bin/gcc
FLAGS=-Wall -g -D_POSIX_SOURCE -std=c11
TARGET=uvgTorrent
INCLUDE=-Isrc/debug -Isrc/macro -Isrc/torrent -Ilib/bencode-tools/include
OBJECTS=

default:
	make clean
	make build
	make test

clean:
	rm -f bin/$(TARGET)

build:
	$(CC) -Wl,-rpath=lib/bencode-tools/ $(INCLUDE) $(OBJECTS) $(FLAGS) -o bin/$(TARGET) src/uvgTorrent.c src/torrent/torrent.c -lm

test:
	valgrind --leak-check=full --track-origins=yes ./bin/$(TARGET) torrents/test.torrent
