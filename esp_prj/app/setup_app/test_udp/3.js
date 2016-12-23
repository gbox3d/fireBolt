/**
 * Created by gbox3d on 15. 7. 16..
 */



// NOTE: the port is different

var dgram = require( "dgram" );
var net = require('net');
var client = dgram.createSocket( "udp4" );

function connect_UDP() {

    var port = 1471;
    var reponse_delay_tick;

    client.on( "message", function( msg, rinfo ) {

        reponse_delay_tick = ((new Date()).getTime() - reponse_delay_tick);

        console.log( "The packet came back : " + reponse_delay_tick );
        console.log('rev : ' + msg);
        loop();
    });
// client listens on a port as well in order to receive ping
    client.bind( port );


    function loop() {

        var message = new Buffer(
            JSON.stringify(
                {cmd:'ping',id : 3}
            )
        );

        setTimeout(function() {

            reponse_delay_tick = (new Date()).getTime();

            client.send(
                message, 0,
                message.length,
                1471, "192.168.9.109" );


        },1000);

    }

    loop();

}

function connect_TCP(option) {

    //var HOST = '192.168.9.40';
    //var PORT = 1470;

    var name,HOST,PORT,timeout_callback;
    name = option.name;
    HOST = option.host;
    PORT = option.port;
    timeout_callback = option.timeout_callback;

    console.log('try ' + HOST)

    var client = new net.Socket();
    client.name = name;
    var ip = HOST;
    var timer_handler = null;
    var timeout_handler = null;

    var q_send_time = 0;

    function loop() {

        console.log('send Q');
        client.write('Q');
        setTimeout(function() {
            loop();
        },1000);
    }

    client.connect(PORT, HOST, function() {

        //client.setTimeout(0);
        console.log('CONNECTED TO: ' + HOST + ':' + PORT);
        //theApp.sockets[name] = client;

        loop();

    });

    client.on('data', function(data) {

        console.log( "network delay :" +    ((new Date()).getTime()  - q_send_time) );
    });

    client.on('close', function() {
        console.log('Connection closed');

    });
    client.on('timeout',function() {
        console.log('timeout');

    });
    client.on('error',function(e) {
        console.log('error');
        console.log(e);
    })
}

connect_TCP({
   name : "",
    host : '192.168.9.109',
    port : 1470,
    timeout_callback : function(err) {
        console.log(err);
    }
});

