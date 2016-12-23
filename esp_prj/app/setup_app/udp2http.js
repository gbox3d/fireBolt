/**
 * Created by gbox3d on 15. 6. 13..
 */

var config = require('./config').config;
var async = require( config.module_path + 'async');

var dgram = require( "dgram" );
var udp_socket = dgram.createSocket( "udp4" );

var theApp ={
    udp : config.udp,
    version : '0.0.8',
    http : config.http,
    udp_rev_callabck : {}
};

//var remote_client;
/*

 udp 는 리스하는 과정이 없다. 연결시작 종료 개념이 없이 바로 데이터 보내고 받는식이다.

 open , close 개념이 없다.

 * */

udp_socket.on( "message", function( msg, rinfo ) {



    try {

        var json_msg = JSON.parse(msg);

        console.log( rinfo.address + ':' + rinfo.port + ' - ' + msg );

        if(theApp.udp_rev_callabck[json_msg.id]) {

            theApp.udp_rev_callabck[json_msg.id].onMessage(json_msg);

        }

    }
    catch(e) {
        //console.log(e)

        //json 포멧이 아니면...
        console.log(msg);

    }



    //remote_client = rinfo;
    //udp_socket.send( Buffer([0x02,0x03]), 0,2, rinfo.port, rinfo.address ); // added missing bracket
});
udp_socket.bind( theApp.udp.port );



////////////////////////////////////
//http
////////////////////////////////////
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
theApp.http_server.listen(theApp.http.port);

console.log('udp2http server v ' + theApp.version );
console.log('  start port : '+ theApp.http.port + ', ready ok!');

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
        case '/scan':
            header['Content-Type'] = 'text/plain';
            res.writeHead(200,header);
            //var remote = JSON.parse( result.query.remote );

            res.end(JSON.stringify( {
                    result : 'ok'
                })
            );
            break;
        case '/send':

            header['Content-Type'] = 'text/plain';
            res.writeHead(200,header);

            //console.log(result.query.remote );
            //var remote = JSON.parse( result.query.remote );

            //console.log(remote);
            //udp_socket.send( Buffer(remote.packet), 0,remote.packet.length, remote.port, remote.address );

            res.end(JSON.stringify( {
                    result : 'ok'
                })
            );

            break;

        default :
            header['Content-Type'] = 'text/plain';
            res.writeHead(200,header);
            res.end(JSON.stringify( {
                result : 'ok',
                msg : 'it is u2u setup broker server ' + theApp.version
            }));
            break;
    }
}

//post 방식으로 처리하기
function process_post(req, res){

    var result = UrlParser.parse(req.url,true);

    //console.log( "url parse data : ");
    //console.log(result);
    //console.log( 'request method :' + req.method);

    var body_data = '';

    //포스트는 데이터가 조각조각 들어 온다.
    req.on('data',function(data) {
        body_data += data;
    });

    //데이터를 다 받았으면
    req.on('end', function () {

        //크로스 도메인 무시
        var header = {
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Methods': 'GET',
            'Access-Control-Max-Age': '1000'
        };
        //POST 경우는 파싱전 인코딩된 문자 되돌려야한다.
        body_data = decodeURIComponent(body_data);

        //console.log( JSON.parse(body_data) );

        switch (result.pathname) {

            case '/send':

                header['Content-Type'] = 'text/plain';

                res.writeHead(200,header);

                var remote = JSON.parse(body_data);

                var strPacket = JSON.stringify(remote.packet);

                //console.log(remote.address);

                async.waterfall([
                        function(next) {

                            console.log("ip : " + remote.address  );
                            console.log("port : " + remote.port  );
                            console.log("packet : " + strPacket  );

                            udp_socket.send( Buffer(strPacket), 0,
                                strPacket.length, remote.port, remote.address );

                            if(remote.packet.id) {

                                var hid = setTimeout(function() {

                                    console.log(remote.packet.id + ' timeout.');
                                    next('timeout')

                                },remote.timeout);

                                theApp.udp_rev_callabck[remote.packet.id] = {
                                    id : remote.packet.id,
                                    onMessage : function(msg) {

                                        clearTimeout(hid);
                                        console.log('send ok');
                                        console.log(msg);

                                        delete theApp.udp_rev_callabck[msg.id];

                                        next(null,msg)

                                    }
                                }
                            }
                            else {
                                next(null,'nocheck')
                            }
                        }
                    ],
                function(err,result) {

                    if(err) {
                        res.end(JSON.stringify( {
                                result : err
                            })
                        );
                    }
                    else {
                        res.end(JSON.stringify( result )
                        );
                    }

                });
                //////async end

                break;

            default :
                //header['Content-Type'] = 'text/plain';
                header['Content-Type'] = 'text/plain';
                res.writeHead(200,header);
                res.end(JSON.stringify( {
                    result : 'ok',
                    msg : 'it is u2u setup broker server ' + theApp.version
                }));
                break;
        }


        //res.write(JSON.stringify({result:'ok'}));
        //
        //res.end();

    });

}

//curl -X POST -H Content-Type:application/json -d '{"address":"192.168.9.109","port":"1471","timeout":500,"packet":{"cmd":"ping","id":666}}'   http://localhost:8080/send
//curl -X POST -H Content-Type:application/json -d '{"address":"192.168.9.108","port":"1471","packet":"{\"cmd\":\"ping\",\"id\":666}"}'   http://localhost:8080/send?remote