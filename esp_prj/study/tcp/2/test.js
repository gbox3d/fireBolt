/**
 * Created by gbox3d on 2016. 4. 2..
 */
var net = require('net');

var HOST = '192.168.4.1';
var PORT = 1471;


function loop() {

    var client = new net.Socket();
    client.connect(PORT, HOST, function() {

        console.log('CONNECTED TO: ' + HOST + ':' + PORT);
        client.write('hello~!');

    });

// Add a 'data' event handler for the client socket
// data is what the server sent to this socket
    client.on('data', function(data) {
        console.log('DATA: ' + data);
    });

// Add a 'close' event handler for the client socket
    client.on('close', function() {
        console.log('Connection closed');
    });

    setTimeout(loop,1000);

}

loop()

