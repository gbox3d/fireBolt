/**
 * Created by gunpower on 2016. 9. 22..
 */

const http = require('http');
const util = require('util');
const fs = require('fs');
const net = require('net');

const os = require('os');
const UrlParser = require('url');

const dgram = require( "dgram" );

let udp_socket = dgram.createSocket( "udp4" );
udp_socket.on( "message", function( msg, rinfo ) {

    console.log( rinfo.address + ':' + rinfo.port + ' - ' + msg );
});
udp_socket.bind(1999);


let theApp = {
    mng_info : {},
    pub_method: {}
}

theApp.mng_info.udp_socket = udp_socket;

theApp.pub_method.send_udp = function (strPacket) {

    udp_socket.send( new Buffer(strPacket), 0, strPacket.length, config.net.udp.port, theApp.mng_info.ip ); // added missing bracket

}

//repl
function setup_repl(context) {

    let repl = require('repl');

    let repl_context = repl.start({
        prompt: 'Node.js via stdin> ',
        input: process.stdin,
        output: process.stdout
    }).context;

//콘텍스트객체 설정
//theApp을 repl에서 볼수있다
    repl_context.theApp = context;
}
setup_repl(theApp);
