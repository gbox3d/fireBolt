/**
 * Created by gunpower on 2016. 10. 25..
 */

let remote = {
    address : '192.168.9.6',
    port : 1471
}


const dgram = require( "dgram" );
const udp_socket = dgram.createSocket( "udp4" );
const fs = require("fs");

//var remote_client;'

var theApp = {
    callbackSeqComplete : function () {

    }

}


udp_socket.on( "message", function( msg, rinfo ) {

    console.log( rinfo.address + ':' + rinfo.port + ' - ' + msg );

    var jsonObj = JSON.parse(msg);

    theApp.callbackSeqComplete(jsonObj.seq)

});
udp_socket.bind( 1472 );


function test() {

    let post_data = fs.readFileSync('three.js',{
        encoding : 'utf8'
    });

    let seq_index = 0;
    let nSize = Buffer.byteLength(post_data);
    let nStep = 900;
    let nStart = 0;
    let nEnd = nStart + nStep;

    let nHandle = null;

    function send_next_step() {
        let tempData =  post_data.slice(nStart,nEnd);

        var pkt = {
            fn : 'fw',
            seq : seq_index,
            data : tempData
        }

        let str_pkt = JSON.stringify(pkt);

        console.log(str_pkt);

        udp_socket.send( Buffer(str_pkt), 0,  Buffer.byteLength(str_pkt), remote.port, remote.address );

        theApp.callbackSeqComplete = function (seq) {

            if(seq == seq_index) {
                clearTimeout(nHandle);
                seq_index++;

                if(nEnd < nSize ) {
                    nStart = nEnd;
                    nEnd = nStart + nStep;
                    if(nEnd > nSize) nEnd = nSize;
                    send_next_step()
                }
                else {
                    console.log("complete");
                }
            }

        }
        nHandle = setTimeout(send_next_step,1000);
    }
    send_next_step();
    console.log('send complete');
}
test();