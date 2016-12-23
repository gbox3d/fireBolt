/**
 * Created by gbox3d on 2014. 1. 25..
 *
 * spark arduino lan shiled simulator
 *
 */
var http = require('http');

var node_module_path = '/usr/local/lib/node_modules/';

var SerialPort = require(node_module_path + 'serialport');

var jsdom = require(node_module_path +'jsdom').jsdom();
var window = jsdom.parentWindow;
var $ = require(node_module_path+ 'jquery')(window);

var theApp = {

    config : {
        base_url : '',
        port : 9304
    },
    deviceID : 'A0001'
}

if(process.argv[2]) {
    theApp.deviceID = process.argv[2];

    console.log(theApp.deviceID);

}


var baudrate = 9600;

//ls /dev | grep usb
var serialPort = new SerialPort.SerialPort(
    "/dev/tty.usbmodem1411", // 맥북용
    {
        baudrate: baudrate
        //parser: serialport.parsers.readline("\r\n") //개행문자기준으로 마샬링해주기
    }
);


serialPort.on("open", function () {

    console.log('open at baudrate :' + baudrate);

    startServer();

    //데이터 읽기
    serialPort.on('data', function(data) {

        console.log(data + "");

    });


//데이터 쓰기
//    serialPort.write("ls\n", function(err, results) {
//        console.log('err ' + err);
//        console.log('results ' + results);
//    });

});


function startServer() {
    function fetchCommand(command) {

        $.ajax(
            {
                url: 'http://' + theApp.config.base_url + ':' + theApp.config.port + '/sparkDeviceCommandExec',
                type: 'GET',
                jsonp: 'callback',
                jsonpCallback: 'callback',
                dataType: 'jsonp',
                data: {
                    id : theApp.deviceID,
                    commandID : command.id

                },
                timeout : 3000,
                success: function(data, textStatus, jqXHR) {
                    console.log(data);

                    if(data.result == 'saveSuccess') {

                        //decodeCommand(command.code);

                            serialPort.write(command.code, function(err, results) {
                                console.log('err ' + err);
                                console.log('results ' + results);
                            });


                    }

                    //다음턴 준비
                    setTimeout(function() {
                        loop();
                    },1000);

                },
                error: function(jqXHR, textStatus) {
                    console.log(textStatus);
                },
                complete: function(jqXHR, textStatus, errorThrown) {

                    console.log('fetch complete : ');


                }
            }
        );


    }

    function loop() {

        var options = {
            hostname: theApp.config.base_url,
            port: theApp.config.port,
            method : 'GET',
            path: '/sparkDeviceCommandCheck?id='+theApp.deviceID
        };

        var req = http.request(options, function(res) {

            console.log('status code :' + res.statusCode);

            res.setEncoding('utf8');

            var body_data = ''

            res.on('data', function (chunk) {
                body_data += chunk;
            });

            //응답을 모두 받고나서 처리해줘야하는것들...
            res.on('end',function() {

                //console.log(body_data);

                body_data = body_data.substr(0,body_data.length-2);

                body_data = body_data.slice(body_data.indexOf('(')+1,body_data.length);

                console.log(body_data);

                var data = JSON.parse(body_data);

                if(data.result == "LoadSuccess") {

                    fetchCommand({
                        id : data.item[0].commandID,
                        code : data.item[0].commandCode
                    });

                }
                else {
                    setTimeout(function() {
                        loop();
                    },1000);
                }

            });

        });

        req.on('error', function(e) {
            console.log('problem with request: ' + e.message);
        });

// write data to request body
//req.write('data\n');
//req.write('data\n');

        req.end();



    }

    loop();

}



