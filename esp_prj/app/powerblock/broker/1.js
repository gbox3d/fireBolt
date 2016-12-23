/**
 * Created by gunpower on 2015. 12. 3..
 */

var http = require('http');
var util = require('util');
var fs = require('fs');
var os = require('os');
var UrlParser = require('url');
var config = require('./config');

var connect = require( config.module_path + 'connect');
var app = connect();

app.resp_obj = {}

app.broker_system = {
    version : '0.0.1',
    units : {
        'test0001' : {
            content:{cmd:"setdio",pin:0,val:1}
        }
    }
}

// respond to all requests
//
//http://localhost:1883/push?id=test0001&content={"cmd":"setdio","pin":0,"val":1}
app.use(
    (function () {
        var req_count = 0;
        return function(req, res, next) {

            //req_count++;
            //console.log('req count : ' + req_count);
            //console.log('step 1');

            try {
                var ip = req.headers['x-forwarded-for'] || req.connection.remoteAddress;

                console.log('remote address:' + ip);

            }
            catch(e)
            {
                console.log(e);

            }

            //console.log(req);

            //크로스 도메인 무시
            var header = {
                'Access-Control-Allow-Origin': '*',
                'Access-Control-Allow-Methods': 'GET',
                'Access-Control-Max-Age': '1000'
            };
            header['Content-Type'] = 'text/plain';
            res.writeHead(200,header);

            app.resp_obj = {}
            next();

        }
    })()
);
//-------------------

app.use('/push', //라우팅
    (function() {
        return function (req,res,next) {
            //res.write(" u routed foo ")
            //console.log('step 2 for foo');
            var result = UrlParser.parse(req.url,true);
            var id = result.query.id;

            if(id) {
                var unit = app.broker_system.units[id];

                if(unit == undefined) {
                    unit = {}
                    app.broker_system.units[id] = unit;
                }

                console.log(result.query.content);
                unit.content = JSON.parse(result.query.content);

                app.resp_obj.result =  'ok';
            }
            else {
                app.resp_obj.result =  'err';
                app.resp_obj.err = 'need id';
            }



            next();
        }
    })()
);
//--
//http://localhost:1883/pop?id=test0001
app.use('/pop',
    (function() {
        return function (req,res,next) {

            var result = UrlParser.parse(req.url,true);
            var id = result.query.id;
            var unit = app.broker_system.units[id];
            if(unit) {
                if(unit.content != undefined) {
                    app.resp_obj.content =  unit.content
                }
                else {

                }
            }
            app.resp_obj.result =  'ok';


            next();
        }
    })()
);


app.use('/help',
    (function() {
        return function (req,res,next) {

            app.resp_obj.msg =  'gunpower block broker sever :' + app.broker_system.version;
            next();
        }
    })()
);



//-------------------
app.use(
    (function () {
        return function(req, res, next) {

            //console.log(this.resp_obj);
            //console.log('step final');
            //console.log('send ok' + JSON.stringify(app.resp_obj));
            res.end(JSON.stringify(app.resp_obj));

        }
    })()
)

var server = http.createServer(app).listen(1883);
console.log('now start powerblock broker : 1883 port v:' + app.broker_system.version)
