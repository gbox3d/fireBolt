import dgram from 'dgram';
const port = 50020;
const server = dgram.createSocket("udp4");
// const networkInterfaces = require('os').networkInterfaces
import { networkInterfaces } from 'os';

server.on("message", function (msg, rinfo) {

    console.log(rinfo);
    // console.log(msg);

    //msg to string
    let msgStr = msg.toString('utf8');
    console.log(msgStr);

    // let _jsonData = JSON.parse(msgStr);

    // console.log(_jsonData); 

    let resMsg = "ping ack";
    let resBuf = Buffer.from(resMsg);
    server.send(resBuf, 0, resBuf.length, rinfo.port, rinfo.address);

});

server.on("listening", function () {
    const address = server.address();
    console.log(`server listening ${address.address}:${address.port}`);
});

server.bind(port);
