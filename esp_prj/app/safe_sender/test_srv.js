var dgram = require("dgram");
var udp_socket = dgram.createSocket("udp4");

//--------------- udp
var local = {
    port: 1471
}

udp_socket.on("message", function(msg, rinfo) {

    console.log('udp receive :' + rinfo.address + ':' + rinfo.port + ' - ' + msg);
    udp_socket.send(new Buffer(msg), 0, msg.length, local.port, rinfo.address)

});
udp_socket.bind(local.port);

console.log('local : bind udp port :' + local.port);

//---------------- tcp

var net = require('net');
var tcp_PORT = 1472;

// Create a server instance, and chain the listen function to it
// The function passed to net.createServer() becomes the event handler for the 'connection' event
// The sock object the callback function receives UNIQUE for each connection
net.createServer(function(socket) {

    // We have a connection - a socket object is assigned to the connection automatically
    console.log('CONNECTED: ' + socket.remoteAddress + ':' + socket.remotePort);

    // Add a 'data' event handler to this instance of socket
    socket.on('data', function(data) {

        console.log('tcp receive ' + socket.remoteAddress + ': ' + data);
        //console.log(data);
        // Write the data back to the socket, the client will receive it as data from the server
        socket.write( new Buffer( 'You said "' + data + '"' + "\n") );

    });

    // Add a 'close' event handler to this instance of socket
    socket.on('close', function(data) {
        console.log('CLOSED: ');
        //console.log(JSON.stringify(socket.getConnection()));
    });

}).listen(tcp_PORT);

console.log('tcp Server listening on ' + ':' + tcp_PORT);