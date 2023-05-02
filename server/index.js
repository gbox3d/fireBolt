import express from 'express'
import dotenv from 'dotenv'
import fs from 'fs-extra'
import https from 'https'
import http from 'http'
import { version as mongodbVer } from 'mongodb/package.json'
import { version as expressVer } from 'express/package.json'
import {version as fireBoltVer} from './package.json'
import { MongoClient } from 'mongodb'

import baseRouter from './routers/base.js'
import logsRouter from './routers/logs.js'

dotenv.config();

(async () => {

    console.log('server start fireBoltVer : ', fireBoltVer);

    const theApp = {
        versions : {
            fireBoltVer,
            mongodbVer,
            expressVer
        },
        WWW_PATH: process.env.WWW_PATH,
        PATH_ROUTER: process.env.PATH_ROUTER,
        SSL: process.env.SSL

    }

    //mongodb setup
    //mongodb 연결
    console.log('MongoDB Node.js Driver Version:', mongodbVer);
    const mongoUrl = process.env.MONGO_URL;

    console.log(`mongoUrl : ${mongoUrl}`);

    try {
        const dbclient = await MongoClient.connect(mongoUrl, { useUnifiedTopology: true });
        console.log(`Connected successfully to server ${mongoUrl} , ${process.env.DB_NAME}`);
        theApp.dbclient = dbclient;
        theApp.dataBase = dbclient.db(process.env.DB_NAME);

    } catch (err) {
        console.error(err);
    }

    //express setup
    const app = express();

    theApp.express = app;

    //express router setup
    if (process.env.WWW_PATH) {
        app.use('/', express.static(process.env.WWW_PATH));
        console.log('static router setup complete ');
    }

    if (process.env.PATH_ROUTER) {

        try {
            let _pathRouters = await fs.readJson(process.env.PATH_ROUTER);

            //라우터 설정
            for (let i = 0; i < _pathRouters.length; i++) {
                app.use('/' + _pathRouters[i].name, express.static(_pathRouters[i].path));

                console.log(`${_pathRouters[i].name} : ${_pathRouters[i].path}`);
            }

            console.log('path router setup complete ');

        }
        catch (err) {
            console.log(err);
        }
    }


    //api router 
    app.use('/api/base', baseRouter(theApp));
    app.use('/api/logs', logsRouter(theApp));


    //순서 주의 맨 마지막에 나온다.
    app.all('*', (req, res) => {
        // res.sendFile(process.env.WWW_PATH + '/build/index.html');
        res
            .status(404)
            .send('oops! resource not found')
    });

    let baseServer;
    if (process.env.SSL === 'True') {
        console.log(`SSL mode ${process.env.SSL}`);
        const options = {
            key: fs.readFileSync(process.env.SSL_KEY),
            cert: fs.readFileSync(process.env.SSL_CERT),
            ca: fs.readFileSync(process.env.SSL_CA),
        };
        // https 서버를 만들고 실행시킵니다
        baseServer = https.createServer(options, app)

    }
    else {
        baseServer = http.createServer({}, app)
    }

    baseServer.listen(process.env.PORT, () => {
        console.log(`server run at : ${process.env.PORT}`)
    });

})();

