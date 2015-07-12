# uvgTorrent

A C Torrent Client - Simon Bursten - https://github.com/unovongalixor
- Supports UDP Protocol ( http://www.libtorrent.org/udp_tracker_protocol.html )

# Requirements

GCC v4.8.2
valgrind v3.10.0

# Purpose

To explore the torrent protocol. uvgTorrent will allow a user to input a magnet link, select a file to stream and then set up a stream which can be viewed via vlc. Bonus points for launching vlc and streaming the correct stream automaticaly.

# Current Status

- Margnet URI is being parsed and connection packet is being sent + received from the trackers

# to do

- Tracker object should conduct it's conversation with the tracker in a seperate thread which it will manage
- Tracker object should send announce request to tracker server and return information on available peers

# Notes

run with "make"
