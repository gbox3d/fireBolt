import express from 'express'
import dotenv from "dotenv"
import fs from 'fs-extra'

import fileControl from "./routers/fileControl.js"
import bolt from "./routers/bolt.js"

async function main() {
    // dotenv.config({ path: './config.env' }); //환경 변수에 등록 
    
    dotenv.config(); //환경 변수에 등록

    console.log(`run mode : ${process.env.NODE_ENV}`);

    //upload 디랙토리 생성 
    {
        let _ = fs.ensureDirSync(process.env.UPLOAD_PATH)
        if (_) {
            console.log(`${_} created`)
        }
    }

    const app = express()

    //auth 인증 
    app.use('/api', (req, res, next) => {

        console.log('check api auth')
        // console.log(req.header('auth-token'))

        let authToken = req.header('auth-token')

        if (authToken === process.env.AUTH_TOKEN) {
            next() //인증성공 다음단계로...
        }
        else {
            res.status(401).json({ r: "err", msg: "auth fail" });
        }

    });

    app.use('/api/v1', fileControl());
    app.use('/api/v1/bolt', bolt());
    

    if (process.env.PATH_ROUTER) {

        try {
            let _pathRouters = await fs.readJson(process.env.PATH_ROUTER);

            //라우터 설정
            for (let i = 0; i < _pathRouters.length; i++) {
                app.use('/' + _pathRouters[i].name, express.static(_pathRouters[i].path));
                console.log(`${_pathRouters[i].name} : ${_pathRouters[i].path}`);
            }
            console.log('static router setup complete ');
        }
        catch (err) {
            console.log(err);
        }
    }

    //순서 주의 맨 마지막에 나온다.
    app.all('*', (req, res) => {
        res
            .status(404)
            .send('oops! resource not found')
    });
    app.listen(process.env.PORT, () => {
        console.log(`server run at : ${process.env.PORT}`)
    })


}

main()