/**
 * Created by gbox3d on 15. 6. 24..
 */


// NOTE: the port is different

var dgram = require( "dgram" );

var client = dgram.createSocket( "udp4" );

client.on( "message", function( msg, rinfo ) {
    console.log( "The packet came back" );
    console.log('rev : ' + msg);
});

// client listens on a port as well in order to receive ping
client.bind( 5638 );