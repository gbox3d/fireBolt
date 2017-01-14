var dgram = require( "dgram" );
var udp_socket = dgram.createSocket( "udp4" );


var theApp = {
    port : 5683
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

    theApp.total_count = 0;
    theApp.err_count = 0;

    return function( msg, rinfo ) {
        if('hellowrold0123456789)!@#$%^&*(AB' != msg) {
            theApp.err_count++;
            console.log( rinfo.address + ':' + rinfo.port + ' - ' + msg );
            console.log('error count : ' + theApp.err_count);
        }
        else {
            //console.log('ok')
        }
        theApp.total_count++;
    }
})()
);
udp_socket.bind( theApp.port );

console.log('start udp :' + theApp.port);
