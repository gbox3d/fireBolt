const net = require('net');
const { off } = require('process');

var theApp = {
    port : 50010
}

net.createServer(function(socket) {

    // We have a connection - a socket object is assigned to the connection automatically
    console.log('CONNECTED: ' + socket.remoteAddress +':'+ socket.remotePort);

    let message = new Buffer.alloc(128);

    let offset = 0;
    message.writeUInt8(0x02, offset++) //STX
    message.writeUInt8(0x1, offset++) //OPCODE 
    message.writeUInt16LE(0, offset) //sr_cnt
    offset += 2
    message.writeUInt32LE(714, offset) //id
    offset+=4;
    message.writeUInt32LE(0, offset) //data size
    offset+=4;

    message.writeUInt8(0x00, offset++) //checksum
    message.writeUInt8(0x03, offset++) //ETX

    socket.write(message.slice(0,offset),()=> {
        console.log(`send wellcome pkt ${offset}`)

    });

    // Add a 'data' event handler to this instance of socket
    socket.on('data', function(data) {

        // console.log('DATA ' + socket.remoteAddress + ': ' + data);
        console.log(data);
        // console.log(`STX : ${data.readUInt8(0)}`)
        // console.log(`opcode : ${data.readUInt8(1)}`)
        // console.log(`sr_count : ${data.readUInt8(2)}`)
        // console.log(`obe_id : ${data.readUInt32LE(4)}`)
        // console.log(`veh_id : ${data.readUInt32LE(8)}`)
        // // Write the data back to the socket, the client will receive it as data from the server
        // socket.write('You said "' + data + '"' + "\n");

    });

    // Add a 'close' event handler to this instance of socket
    socket.on('close', function(data) {
        console.log('CLOSED: ');
        //console.log(JSON.stringify(socket.getConnection()));
    });

}).listen(theApp.port);

//console.log('Server listening on ' + HOST +':'+ PORT);
console.log(`Server listening on : ${theApp.port}`);
