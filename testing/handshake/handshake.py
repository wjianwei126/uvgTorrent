'''
import socket

def generate_handshake(info_hash, peer_id):
	""" Returns a handshake. """

	protocol_id = "BitTorrent protocol"
	len_id = str(len(protocol_id))
	reserved = "00000000"

	return len_id + protocol_id + reserved + info_hash + peer_id

def send_recv_handshake(handshake, host, port):
	""" Sends a handshake, returns the data we get back. """

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host, port))
	print(handshake)
	s.send(handshake)

	data = s.recv(len(handshake))
	print(data)
	s.close()

	return data

send_recv_handshake(generate_handshake("9609f0336566953f3bf342241b25e2437f65b2c8".encode("hex"), "12345678901234567890"), "127.0.0.1", 51413)
'''

import socket
import struct
import bencode

def generate_handshake(info_hash, peer_id):
	""" Returns a handshake. """

	protocol_id = "BitTorrent protocol"
	len_id = str(len(protocol_id))
	reserved = "00000000"

	packet = struct.pack('c19s8s20s20s', chr(19), "BitTorrent protocol", "\x00\x00\x00\x00\x00\x10\x00\x00", info_hash, peer_id)
	print(len(packet))

	#f = open('packet','w')
	#f.write(packet) # python will convert \n to os.linesep
	#f.close()

	return packet

def send_recv_handshake(handshake, host, port):
	""" Sends a handshake, returns the data we get back. """

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host, port))
	print("SENT     :: " + handshake)
	s.send(handshake)

	data = s.recv(len(handshake))
	print("RESPONSE :: " + data)

	return s

def send_recv_metadata(s):
	# METADATA HANDSHAKE
	data = bencode.bencode({"m":{"ut_metadata": 1}})
	data = chr(20) + chr(0) + data

	print("SENT     :: " + data)
	s.send(data)

	data = s.recv(4096)
	print("RESPONSE :: " + data)

	# METADATA REQUEST

	s.close()
	pass



sock = send_recv_handshake(generate_handshake("9609f0336566953f3bf342241b25e2437f65b2c8".decode("hex"), "12345678901234567890"), "127.0.0.1", 51413)
send_recv_metadata(sock)