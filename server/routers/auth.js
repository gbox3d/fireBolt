import express from 'express'
import cors from 'cors'
import moment from 'moment'
import { ObjectId } from 'mongodb'

import jwt from 'jsonwebtoken';

export default function (_Context) {

    const dataBase = _Context.dataBase;

    const router = express.Router()
    //cors 정책 설정 미들웨어 
    router.use(cors());

    router.use(express.raw({ limit: '500kb' })) //파일용량 1기가 바이트로 제한
    router.use(express.json({ limit: '500kb' })) //json 바디 미들웨어, content-type : application/json 일 경우 req.body로 받아온다.
    router.use(express.text({ limit: '500kb' })) //text 바디 미들웨어, content-type : application/text 일 경우 req.body로 받아온다.

    router.route('/').get((req, res) => {
        res.json({ r: 'ok', info: `daisy auth system` })
    });

    // 토큰을 발급하는 라우터
    router.post('/login', async (req, res) => {
        try {
            let result = await dataBase.collection('users').findOne({
                userId: req.body.userId,
                userPw: req.body.userPw
            })

            if (result) {
                let token = jwt.sign({
                    userId: result.userId,
                    userName: result.userName,
                    _id : result._id
                }, process.env.JWT_SECRET, { expiresIn: '14d' });

                return res.json({ r: 'ok', info: { token: token } })
            }
            else {
                return res.json({ r: 'fail', info: 'userId or userPw is not correct' })
            }
        }
        catch (error) {
            console.error(error);
            return res.status(500).json({
                code: 500,
                message: '서버 에러',
            });
        }
    });


    return router

}