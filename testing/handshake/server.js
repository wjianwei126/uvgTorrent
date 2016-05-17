var pwp = require('peer-wire-protocol');
var net = require('net');

net.createServer(function(socket) {
    var wire = pwp();

    // pipe to and from the protocol
    socket.pipe(wire).pipe(socket);

    wire.on('handshake', function(infoHash, peerId) {
        // lets emit a handshake of our own as well
        wire.handshake(new Buffer(infoHash), new Buffer(peerId));
    });

    wire.on('unchoke', function() {
        console.log('peer is no longer choking us: '+wire.peerChoking);
    });
}).listen(12321);
