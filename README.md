# uvgTorrent

A C Torrent Client 

Simon Bursten 
	(unovongalivor - https://github.com/unovongalixor/uvgTorrent)

- Supports UDP Protocol ( http://www.libtorrent.org/udp_tracker_protocol.html )

# Requirements

GCC v4.8.2
valgrind v3.10.0

# Purpose

To explore the torrent protocol. uvgTorrent will allow a user to input a magnet link, select a file to stream and then set up a stream which can be viewed via vlc. Bonus points for launching vlc and streaming the correct stream automaticaly.

# Notes

structs are being used to mimic instances.

class structure is as follows:

Torrent
  ↳ Trackers (stored in a linked list)
  	  ↳ Peers (stored in hash table)
  ↳ Chunks (retrieved from peers, but managed in torrent class)