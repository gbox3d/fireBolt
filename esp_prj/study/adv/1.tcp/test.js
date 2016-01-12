/**
 * Created by gunpower on 2015. 12. 3..
 */

var net = require('net');
var repl = require('repl');

var HOST = '192.168.9.20';
var PORT = 2323;


theApp = {}

var repl_context = repl.start({
    prompt: 'Node.js via stdin> ',
    input: process.stdin,
    output: process.stdout
}).context;

//콘텍스트객체 설정
//theApp을 repl에서 볼수있다
repl_context.theApp = theApp;

var client = new net.Socket();
theApp.client = client;
client.connect(PORT, HOST, function() {

    console.log('CONNECTED TO: ' + HOST + ':' + PORT);

    //client.write('I am Chuck Norris!');

    // Write a message to the socket as soon as the client is connected, the server will receive it as message from the client

    //client.write('hello~!');
    //client.write('한글','utf-8',function() {
    //
    //    console.log();
    //
    //});
    //
    ////안드로이드 소켓통신할때는 개행문자가 필요한 경우도 있다.
    //client.write('\n','utf-8',function() {
    //
    //    console.log('개행 문자 전송');
    //
    //});



});

var prev_tick = (new Date()).getTime();
// Add a 'data' event handler for the client socket
// data is what the server sent to this socket
client.on('data', function(data) {

    var cur_tick = (new Date()).getTime()
    console.log(cur_tick - prev_tick + ' : ' + data)
    prev_tick = cur_tick;

    //console.log(data);
    // Close the client socket completely
    //client.destroy();

});

// Add a 'close' event handler for the client socket
client.on('close', function() {
    console.log('Connection closed');
});