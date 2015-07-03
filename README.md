# uvgTorrent

An Educational Torrent Client

# Purpose

To explore the torrent protocol as well as c project developemnt. uvgTorrent will allow a user to input a magnet link, select a file to stream and then set up a stream which can be viewed via vlc. Bonus points for launching vlc and streaming the correct stream automaticaly.

Special focus on:

- Using structs to mimic types (see torrent/torrent.h). While this likely undermines performance by accessing a function from a struct's location + offset it makes my life much easier as a developer and makes the code cleaner.