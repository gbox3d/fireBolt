var dgram = require( "dgram" );
var udp_socket = dgram.createSocket( "udp4" );


var theApp = {
    port : 1471
}
////////////------------------//////////////////////
var    repl = require('repl')

var repl_context = repl.start({
    prompt: 'Node.js via stdin> ',
    input: process.stdin,
    output: process.stdout
}).context;

//콘텍스트객체 설정
//theApp을 repl에서 볼수있다
repl_context.theApp = theApp;
//////////////////

udp_socket.on( "message", (function() {

    return function( msg, rinfo ) {

        console.log(msg.toString());
        console.log( rinfo.address + ':' + rinfo.port)

    }
})()
);
udp_socket.bind( theApp.port );

function sendtest(port) {
    var message = new Buffer("hello");
    udp_socket.send(
        message, 0,
        message.length,
        //33333, "127.0.0.1" );
        port, "192.168.9.20" );

}

theApp.sendtest = sendtest;


console.log('start udp :' + theApp.port);
