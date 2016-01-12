var port = 5683;
var dgram = require( "dgram" );
var udp_socket = dgram.createSocket( "udp4" );

udp_socket.on( "message", function( msg, rinfo ) {

    console.log( rinfo.address + ':' + rinfo.port + ' - ' + msg );

    //udp_socket.send( Buffer('hello'), 0,5, remote.port ,remote.address); // added missing bracket
});

udp_socket.bind( port );
console.log('start udp :' + port);
