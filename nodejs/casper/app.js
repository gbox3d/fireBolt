/**
 * Created by gunpower on 2016. 6. 21..
 */

var config = require("./config");
var dgram = require( "dgram" );
var client = dgram.createSocket("udp4");
var async = require( config.module_path + 'async');
var http = require('http');

var os = require('os');
var ifaces = os.networkInterfaces();

theApp = {
    broad_cast_address : config.address,
    port : config.port,
    cast_delay : 5000,
    cast_loop : true,
    network_device : {}
};


function loop() {

    //console.log(config.select_device);

    async.waterfall([
            //브로드 캐스팅
            function (next) {
                var info = {
                    network_device : theApp.network_device[config.select_device]
                }
                var message = new Buffer( JSON.stringify(info) );
                client.send(message, 0, message.length, theApp.port, theApp.broad_cast_address);
                next();

            }
        ],
        function (err,result) {

            if(err) {
                console.log(err);
            }

            if(theApp.cast_loop) {
                setTimeout(loop,theApp.cast_delay);
            }

        }
    );

}


async.waterfall(
    [
        //통신 디바이스 정보 얻기
        function (next) {
            console.log(ifaces);

            Object.keys(ifaces).forEach(function (ifname) {
                var alias = 0;

                ifaces[ifname].forEach(function (iface) {
                    if ('IPv4' !== iface.family || iface.internal !== false) {
                        // skip over internal (i.e. 127.0.0.1) and non-ipv4 addresses
                        //next();
                    }
                    else if( 'IPv4' == iface.family ) {
                        if (alias >= 1) {
                            // this single interface has multiple ipv4 addresses
                            console.log(ifname + ':' , iface.address);

                        } else {
                            // this interface has only one ipv4 adress
                            console.log(ifname, iface.address);
                            //next();
                        }

                        theApp.network_device[ifname] =  iface.address;

                    }

                    ++alias;
                });
            });

            next();

        },
        //udp 바운딩
        function (next) {
            client.bind(
                function() {
                    console.log('udp bind success');
                    client.setBroadcast(true);
                    loop();
                    next()
                }
            );
        }

    ],
    function (err, result) {



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
)





