# uvgTorrent

A C Torrent Client - Simon Bursten - https://github.com/unovongalixor
- Supports UDP Protocol ( http://www.libtorrent.org/udp_tracker_protocol.html )

# Requirements

- GCC v4.8.2
- valgrind v3.10.0
- UNIX based OS (Ubuntu, etc) 

# Purpose

To explore the torrent protocol. uvgTorrent will allow a user to input a magnet link, select a file to stream and then set up a stream which can be viewed via vlc. Bonus points for launching vlc and streaming the correct stream automaticaly.

# Current Status

- Margnet URI is being parsed and connection packet is being sent + received from the trackers

# to do

- Tracker object should conduct it's conversation with the tracker via a ThreadPool (yet to be developed) which will manage cross thread communication using queues.
- Tracker object should send announce request to tracker server and return information on available peers
- Clean up container destroy functions. Linkedlist and Hashtable types can store conplex structs which need to be destroyed by calling the destroy method to make sure any internaly stored data is also free'd. I'm thinking of creating 2 destroy functions for these structures - destroy_free (called when you want to call free on stored data) and destroy (called when you want to call destroy on stored data)

# Notes

run with "make". will run via valgrind
