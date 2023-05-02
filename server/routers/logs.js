import express from 'express'
import cors from 'cors'
import moment from 'moment'
import { ObjectId } from 'mongodb'
////////////////////////////////////////////////////////////////////////////////////////////
/* 

url base is /com/logs/...

*/
////////////////////////////////////////////////////////////////////////////////////////////

const version = '0.0.1'

export default function ({ dbclient }) {

    const router = express.Router()

    const collectionName = 'logs'

    //cors 정책 설정 미들웨어 
    router.use(cors());

    //raw 바디 미들웨어, content-type : application/octet-stream 일 경우 req.body로 받아온다.
    router.use(express.raw({ limit: '500kb' })) //파일용량 1기가 바이트로 제한
    router.use(express.json({ limit: '500kb' })) //json 바디 미들웨어, content-type : application/json 일 경우 req.body로 받아온다.
    router.use(express.text({ limit: '500kb' })) //text 바디 미들웨어, content-type : application/text 일 경우 req.body로 받아온다.
    router.use(express.urlencoded({ extended: true })) //urlencoded 바디 미들웨어, content-type : application/x-www-form-urlencoded 일 경우 req.body로 받아온다.

    router.route('/').get((req, res) => {
        res.json({ r: 'ok', info: `log system v ${version}` })
    });

    //로그 등록
    router.route('/insert').post(async (req, res) => {
        console.log(req.body);
        const query = req.body;
        query.rdate = moment().format('YYYY-MM-DD HH:mm:ss');
        query.address = req.ip;
    
        try {
          const result = await dbclient
            .db(process.env.DB_NAME)
            .collection(collectionName)
            .insertOne(query);
          res.json({
            r: 'ok',
            data: {
              sid: result.insertedId,
              rdate: query.rdate,
            },
          });
        } catch (err) {
          console.log(err);
          res.json({ r: 'fail', info: err });
        }
      });
    
    //로그 리스트
    router.route('/list').post(async (req, res) => {
        let query = req.body;
        let skip = (query.page - 1) * query.unit || 0;
        let limit = query.unit || 10;
    
        try {
            // 전체 로그 개수 조회
            const count = await dbclient.db(process.env.DB_NAME).collection(collectionName).countDocuments(query.option);
    
            const lastPage = Math.ceil(count / limit);
    
            // 전체 페이지 수 계산
            const pageCount = Math.ceil(count / limit);
    
            // 현재 페이지가 전체 페이지 수보다 큰 경우 마지막 페이지에 해당하는 리스트 조회
            if (query.page > pageCount && pageCount > 0) {
                // query.page = pageCount;
                skip = (pageCount - 1) * query.unit || 0;
            }
    
            //get list array
            const _list = await dbclient.db(process.env.DB_NAME).collection(collectionName)
                .find(query.option,
                    {
                        projection: {
                            title: 1,
                            _id: 1,
                            rdate: 1,
                            tags : 1,
                        }
                    })
                .sort({ rdate: -1 })
                .skip(parseInt(skip))
                .limit(parseInt(limit))
                .toArray();
    
            console.log(`find count ${_list.length} ok`);
            res.json({ r: 'ok', list: _list, count: count, lastPage: lastPage });
        } catch (err) {
            console.log(err);
            res.json({ r: 'fail', data: err });
        }
    });
    
    //detail
    router.route('/detail').post(async (req, res) => {
        let query = req.body;
        console.log(`/detail , id : ${query.id}`);
    
        try {
            const result = await dbclient.db(process.env.DB_NAME)
                .collection(collectionName)
                .findOne({
                    _id: new ObjectId(query.id)
                    // creator: req.decoded.userId
                });
    
            console.log(result);
            res.json({ r: 'ok', data: result });
        } catch (err) {
            console.log(err);
            res.json({ r: 'fail', data: err });
        }
    });
    
    router.route('/delete').post(async (req, res) => {
        // console.log(req.body)
        let query = req.body

        // console.log(`/clear , sid : ${query.id}`)
        console.log(`/clear , query : ${JSON.stringify(query)}`)

        if(query._id){
            query._id = new ObjectId(query._id)
        }

        try {
            let result = await dbclient.db(process.env.DB_NAME)
                .collection(collectionName)
                .deleteMany(
                    query
                )

            console.log(`delete count ${result.deletedCount} ok`)
            res.json({ r: 'ok', data: `delete count : ${result.deletedCount}` })
        }
        catch (err) {
            console.log(err)
            res.json({ r: 'fail', info: err })
        }
    });



    return router
}