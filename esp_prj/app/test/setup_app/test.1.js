/**
 * Created by gbox3d on 15. 6. 14..
 */


// NOTE: the port is different
var host = "127.0.0.1", port = 1471;

var dgram = require( "dgram" );

var client = dgram.createSocket( "udp4" );

client.on( "message", function( msg, rinfo ) {
    console.log( "The packet came back" );
    console.log('rev : ' + msg);
});

// client listens on a port as well in order to receive ping
client.bind( port );

//var message = new Buffer( JSON.stringify({cmd:'echo',msg:'hello'}) );

var message = new Buffer( JSON.stringify(
        {cmd:'save',
            name:'init.lua',
            content : ''
            //content : 'test'
            //content : 'wifi.setmode(wifi.STATION) cfg = {ip = "192.168.9.108",netmask = "255.255.255.0",gateway="192.168.9.177"} wifi.sta.setip(cfg) wifi.sta.connect() wifi.sta.config("GUNPOWER_PI_1","")'
        })
);

client.send(
    message, 0,
    message.length,
    //33333, "127.0.0.1" );
    1471, "192.168.9.108" );