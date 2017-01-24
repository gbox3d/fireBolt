/**
 * Created by gunpower on 2016. 5. 5..
 */

var dgram = require( "dgram" );
var udp_socket = dgram.createSocket( "udp4" );


////////////------------------//////////////////////
var    repl = require('repl')

var theApp = {
    testMsg : 'hello repl',
    udp_socket : udp_socket
}
theApp.sendTest = function(pin,val) {
    var cmd = {
        fn : 'gpio-set',
        pin : pin,
        val : val
    }
    // var cmd = {
    //         fn : 'test'
    //     }
    cmd = JSON.stringify(cmd)
    udp_socket.send( new Buffer( cmd), 0,cmd.length, 2714,"192.168.4.1");
}

var repl_context = repl.start({
    prompt: 'Node.js via stdin> ',
    input: process.stdin,
    output: process.stdout
}).context;

//콘텍스트객체 설정
//theApp을 repl에서 볼수있다
repl_context.theApp = theApp;