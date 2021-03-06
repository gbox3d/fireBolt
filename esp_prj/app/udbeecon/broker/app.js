/**
 * Created by gunpower on 2016. 4. 13..
 */

var config = require('./config');

var theApp = {
    version : '0.0.3',
    broadcast : {
        port : 2714
    },
    device_list : {},
    api_port : config.http_port
}


var dgram = require( "dgram" );
var udp_socket = dgram.createSocket( "udp4" );

//var remote_client;
/*

 udp 는 리스하는 과정이 없다. 연결시작 종료 개념이 없이 바로 데이터 보내고 받는식이다.

 open , close 개념이 없다.

 * */
//80414243444546474881
udp_socket.on( "message", function( msg, rinfo ) {

    //console.log( 'remote :' + rinfo.address + ':' + rinfo.port + ' - ' + (new Date()).getTime() );
    //console.log(msg.toString());
    //remote.address = rinfo.address;
    try {
        var obj = JSON.parse(msg.toString());

        if(obj.pkt == 'bcast') {
            //console.log(obj);
            if(obj.device_id) {
                theApp.device_list[obj.device_id] = {
                    device_id : obj.device_id,
                    ip : rinfo.address
                }

                var strPacket = JSON.stringify({
                    fn : 'checkin',
                    ip : theApp.ipAddress
                })

                udp_socket.send( new Buffer(strPacket), 0, strPacket.length, theApp.broadcast.port ,rinfo.address);
            }
        }
        else if(obj.pkt == 'stm') {
            console.log(obj)

        }

    }
    catch (e) {
        console.log(e)
    }



});
udp_socket.bind( theApp.broadcast.port );
console.log('local : bind udp port :' +  theApp.broadcast.port );

//디바이스 리스트 클리어
(function scanLoop() {
    function _scanLoop() {
        //console.log('clear list')
        //console.log(theApp.device_list);
        theApp.device_list = {};
        setTimeout(_scanLoop,30000);
    }
    _scanLoop();
})();


/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------
var http = require('http');
var async = require(config.module_path + 'async');
var http = require('http');
var util = require('util');
var fs = require('fs');
var os = require('os');
var UrlParser = require('url');

theApp.http_server = http.createServer(
    function(req, res){
        switch(req.method){
            case 'GET':
                process_get(req, res);
                break;
            case 'POST':
                process_post(req, res);
                break;
        }
    }
);
theApp.http_server.listen(theApp.api_port);

console.log('>udbCon broker web simulator ' + theApp.version );
console.log('>start port : '+ theApp.api_port + ', ready ok!');

//get 처리 해주기
function process_get(req, res){

    var result = UrlParser.parse(req.url,true);

    //크로스 도메인 무시
    var header = {
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'GET',
        'Access-Control-Max-Age': '1000'
    };

    switch (result.pathname) {
        case '/echo':
            header['Content-Type'] = 'text/plain';
            res.writeHead(200,header);
            res.end(JSON.stringify( {
                    result : 'ok',
                    msg : result.query.msg
                })
            );

            break;
        case '/get-device-list':
            (function() {
                header['Content-Type'] = 'text/plain';

                console.log(theApp.device_list);

                res.writeHead(200,header);
                res.end(JSON.stringify( theApp.device_list ) );

            })();
            //------//
            break;
        case '/send-msg':
            (function() {
                header['Content-Type'] = 'text/plain';
                res.writeHead(200,header);

                var device = theApp.device_list[result.query.device_id];
                var rst = {result : 'ok'}

                if(device) {
                    console.log(device.ip + ',' + result.query.msg)
                    udp_socket.send( new Buffer(result.query.msg), 0,result.query.msg.length, theApp.broadcast.port ,device.ip);
                }
                else {
                    rst.result = 'device not found';
                }

                res.end(
                    JSON.stringify(rst)
                );

            })();

            break;

        default :
            header['Content-Type'] = 'text/plain';
            res.writeHead(200,header);
            res.end(JSON.stringify( {
                result : 'ok',
                msg : 'it is T400 Simulator ' + theApp.version
            }));
            break;
    }
}

//post 방식으로 처리하기
function process_post(req, res){

}

////////////------------------//////////////////////
var    repl = require('repl')

theApp.sendTest = function() {
    //udp_socket.send( Buffer('hello'), 0,5, theApp.broadcast.port ,theApp.remote.address);
}

var repl_context = repl.start({
    prompt: 'Node.js via stdin> ',
    input: process.stdin,
    output: process.stdout
}).context;

//콘텍스트객체 설정
//theApp을 repl에서 볼수있다
repl_context.theApp = theApp;

///////////////////////
//주소출력
function displayAddress() {
    var os = require('os');
    var ifaces = os.networkInterfaces();
//console.log(ifaces);

    Object.keys(ifaces).forEach(function (ifname) {
        var alias = 0;

        ifaces[ifname].forEach(function (iface) {
            if ('IPv4' !== iface.family || iface.internal !== false) {
                // skip over internal (i.e. 127.0.0.1) and non-ipv4 addresses
                return;
            }

            if (alias >= 1) {
                // this single interface has multiple ipv4 addresses
                console.log(ifname + ':' + alias, iface.address);
            } else {
                // this interface has only one ipv4 adress
                console.log(ifname, iface.address);
                theApp.ipAddress = iface.address;
            }
            ++alias;
        });
    });
}

displayAddress();
