import net from 'net';

const theApp = {
    port : 50070
}

net.createServer(function(socket) {

    // We have a connection - a socket object is assigned to the connection automatically
    console.log('CONNECTED: ' + socket.remoteAddress +':'+ socket.remotePort);

    let message = new Buffer.alloc(128);

    // let offset = 0;
    // message.writeUInt8(0x02, offset++) //STX
    // message.writeUInt8(0x1, offset++) //OPCODE 
    // message.writeUInt16LE(0, offset) //sr_cnt
    // offset += 2
    // message.writeUInt32LE(714, offset) //id
    // offset+=4;
    // message.writeUInt32LE(0, offset) //data size
    // offset+=4;

    // message.writeUInt8(0x00, offset++) //checksum
    // message.writeUInt8(0x03, offset++) //ETX

    // socket.write(message.slice(0,offset),()=> {
    //     console.log(`send wellcome pkt ${offset}`)
    // });

    // Add a 'data' event handler to this instance of socket
    
    socket.on('data', function(data) {

        //append data to buffer
        // message = Buffer.concat([message,data]);
        
        console.log(data.toString('utf8'));
    });

    // Add a 'close' event handler to this instance of socket
    socket.on('close', function(data) {
        console.log('CLOSED: ');
        //console.log(JSON.stringify(socket.getConnection()));
    });

}).listen(theApp.port);

//console.log('Server listening on ' + HOST +':'+ PORT);
console.log(`Server listening on : ${theApp.port}`);