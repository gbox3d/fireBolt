/**
 * Created by gbox3d on 15. 6. 22..
 */

var dgram = require( "dgram" );

var theApp = {
    version : '0.0.1',
    port : 5638,
    http: {
        port : 5680
    }
}

///////////////
//데이터 받기

var strBuf = '';
var balance_data = [512,512];

var udp_data_socket = dgram.createSocket( "udp4" );
udp_data_socket.on( "message", function( msg, rinfo ) {

    //console.log(  rinfo.address + ':' + rinfo.port + ' - ' + msg );


    for(var i=0; i < msg.length;i++) {

        if(msg[i] == 0x81) {

            //console.log(strBuf)
            balance_data = JSON.parse( '[' + strBuf  +']')
            //console.log(balance_data);
            strBuf = '';

        }
        else {
            strBuf += String.fromCharCode(msg[i]);
        }
    }



});

udp_data_socket.bind(theApp.port)

///////////////
console.log('balance data receiver v' + theApp.version);
console.log('listen at :' + theApp.port);


////////////////////////////////////
//http
////////////////////////////////////
var http = require('http');
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

console.log('balance app server v ' + theApp.version );
console.log('start rest api port : '+ theApp.http.port + ', ready ok!');

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
        case '/data':

            header['Content-Type'] = 'text/plain';

            res.writeHead(200,header);

            res.end(JSON.stringify( {
                    result : 'ok',
                    data : balance_data
                })
            );
            break;


        default :
            header['Content-Type'] = 'text/plain';
            res.writeHead(200,header);
            res.end(JSON.stringify( {
                result : 'ok',
                msg : 'it is balance app server ' + theApp.version
            }));
            break;
    }
}

//post 방식으로 처리하기
function process_post(req, res){

    var result = UrlParser.parse(req.url,true);
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

            default :
                header['Content-Type'] = 'text/plain';
                res.writeHead(200,header);
                res.end(JSON.stringify( {
                    result : 'ok',
                    msg : 'it is balance app server ' + theApp.version
                }));
                break;
        }



    });

}
