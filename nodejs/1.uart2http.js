/**
 * Created by gunpower on 2016. 6. 17..
 */
var theApp = {
    version : '1.0.0',
    appName : 'http2uart app',
    module_path : '/usr/local/lib/node_modules/',
    port : 10010,
    inputQue : {
        buf : new Buffer(256),
        index : 0,
        push : function (data) {
            if(this.index >= 256) {
                this.index = 0;
            }
            this.buf[this.index++] = data;

        },
        pop : function () {
            var index = this.index;
            this.index = 0;
            return this.buf.slice(0,index)
        }
    }
};

function CreateProcessHandler() {

    var header = {
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'GET',
        'Access-Control-Max-Age': '1000'
    };

    return {
        '/system-info' : function(result,res,context) {
            header['Content-Type'] = 'text/plain';
            res.writeHead(200,header);
            res.end(JSON.stringify( {
                    result : 'ok',
                    msg : theApp.appName + ' Ver.' + theApp.version
                })
            );

        },
        '/pop' : function (result,res,context) {
            header['Content-Type'] = 'text/plain';

            var index = theApp.inputQue.index;


            res.writeHead(200,header);
            res.end(
                JSON.stringify( {
                    result : 'ok',
                    content : theApp.inputQue.pop()
                })
            );



        },
        'default' : function (result,res) {
            header['Content-Type'] = 'text/plain'
            res.writeHead(200,header);
            res.end(JSON.stringify( {
                    result : 'ok',
                    msg : 'can not process :' + result.pathname
                })
            );
            console.log('can not process :' + result.pathname )

        }
    }
}


//----------------------------setup-----------------------------------------//
var async = require( theApp.module_path + 'async');
async.waterfall([
        function (next) {
            var serialport = require("/usr/local/lib/node_modules/serialport");
            var baudrate = 115200;

//ls /dev | grep usb
            var serialPort = new serialport.SerialPort(
                //"/dev/ttyACM0",
                //"/dev/ttyAMA0", //UART 씨리얼
                //"/dev/ttyUSB0", // 유에스비 씨리얼 어댑터 인듯
                "/dev/tty.usbserial",
                {
                    baudrate: baudrate
                    //parser: serialport.parsers.readline("\r\n") //개행문자기준으로 마샬링해주기
                }
            );


            serialPort.on("open", function () {
                console.log('open at baudrate :' + baudrate);
                next();
                //setTimeout(function() {
                //
                //    serialPort.write('!0113000.',function() {});
                //
                //},6000);
                //데이터 읽기
                serialPort.on('data', function(data) {
                    //console.log(data );
                    for(var i=0;i<data.length;i++) {
                        theApp.inputQue.push(data[i]);
                        //theApp.inputQue.buf[theApp.inputQue.index++] = data[i];
                    }

                });

            });

        },
        function(next) {

            theApp.ProcessesHandlers = CreateProcessHandler();
            next();

        },
        function (next) {
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
            theApp.http_server.listen(theApp.port);

            console.log('log server :' + theApp.port + ',version :' + theApp.version);

//get 처리 해주기
            function process_get(req, res) {
                var result = UrlParser.parse(req.url,true);
                var handler = theApp.ProcessesHandlers[result.pathname];
                if(handler) {
                    handler(result,res,theApp);
                }
                else {
                    theApp.ProcessesHandlers['default'](result,res)
                }

            }

//post 방식으로 처리하기
            function process_post(req, res){

            }

            next();


        }
    ],
    function(err,result) {


        if(err) {
            console.log(err);
        }


        function setup_repl(context) {

            var repl = require('repl');

            var repl_context = repl.start({
                prompt: 'Node.js via stdin> ',
                input: process.stdin,
                output: process.stdout
            }).context;

//콘텍스트객체 설정
//theApp을 repl에서 볼수있다
            repl_context.theApp = context;
        }
        setup_repl(theApp);

    }
);