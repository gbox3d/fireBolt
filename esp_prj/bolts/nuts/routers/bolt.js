import express from 'express'
import dgram from 'dgram';

export default () => {

    const router = express.Router()

    //cors 정책 설정 미들웨어 
    router.use((req, res, next) => {

        res.set('Access-Control-Allow-Origin', '*'); //cors 전체 허용
        res.set('Access-Control-Allow-Methods', '*');
        res.set("Access-Control-Allow-Headers", "*");

        console.log(req.header('content-type'))
        console.log(`check file control mw auth ${req.originalUrl}`)
        next()
    })

    //raw 바디 미들웨어, content-type : application/octet-stream 일 경우 req.body로 받아온다.
    router.use(express.raw({ limit: '500kb' })) //파일용량 1기가 바이트로 제한
    router.use(express.json()) //json 바디 미들웨어, content-type : application/json 일 경우 req.body로 받아온다.
    router.use(express.text()) //text 바디 미들웨어, content-type : application/text 일 경우 req.body로 받아온다.
    router.use(express.urlencoded({ extended: true })); // URL 인코딩된 바디를 파싱하기 위한 미들웨어 (form submit 처리)

    //udp nuts device server
    const nuts_udp_server = dgram.createSocket("udp4");


    nuts_udp_server.on('error', (err) => {
        console.log(`server error:\n${err.stack}`);
        nuts_udp_server.close();
    });

    // 디바이스 정보를 저장할 객체
    const devices = {};

    //receive message postbox
    const postbox = {};

    nuts_udp_server.on('message', (message, rinfo) => {
        // console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
        //#BC_deviceid

        //msg
        const msg = message.toString()

        if (msg.startsWith('#BC_')) {

            let _msg = msg.toString().split('_')
            let _deviceid = _msg[1]

            if (devices[_deviceid]) {
                // console.log(`device ${_deviceid} is already registered`)
                devices[_deviceid].at = new Date().getTime()
            }
            else {
                //create new device
                console.log(`device ${_deviceid} is new`)
                devices[_deviceid] = {
                    address: rinfo.address,
                    port: rinfo.port,
                    at: new Date().getTime()
                }
            }
        }
        else if (msg.startsWith('#RES_')) {
            console.log(`res msg from ${rinfo.address}:${rinfo.port} : ${msg}`)

            const _key = rinfo.address + ":" + rinfo.port

            if (postbox[_key]) {
                postbox[_key](message)
                delete postbox[_key]
            }
        }
        else if(msg.startsWith('#POST_')){
            
            // console.log(msg)
            // const _key = rinfo.address + ":" + rinfo.port

            //sample : #POST_devid_TempSensor_26.5
            let _msg = msg.toString().split('_')
            let _deviceid = _msg[1]
            let _key = _msg[2]
            let _value = _msg[3]

            if(devices[_deviceid]){
                devices[_deviceid][_key] = _value
                devices[_deviceid].at = new Date().getTime()

                console.log(`device ${_deviceid} is updated ${_key} : ${_value}`)
            }
            else {
                // create new device
                devices[_deviceid] = {
                    address: rinfo.address,
                    port: rinfo.port,
                    at: new Date().getTime()
                }
                devices[_deviceid][_key] = _value

                console.log(`device ${_deviceid} is new ${_key} : ${_value}`)
            }
        }

    });

    nuts_udp_server.on('listening', () => {
        const address = nuts_udp_server.address();
        console.log(`server listening ${address.address}:${address.port}`);
    });

    nuts_udp_server.bind(process.env.NUTS_UDP_PORT);

    //api rounter

    router.route('/').get((req, res) => {
        res.json({ r: 'ok', info: 'bolt manager' })
    })

    router.route('/device').get((req, res) => {
        console.log(devices)
        res.json({ r: 'ok', devices: devices })
    })

    router.route('/cmd').post((req, res) => {

        let _deviceid = req.body.deviceid
        let _cmd = req.body.cmd

        if (devices[_deviceid]) {

            let _msg = Buffer.from(_cmd)

            try {
                let hTimer = setTimeout(() => {

                    try {
                        if (postbox[_key]) {
                            res.json({ r: 'err', msg: 'timeout' })
                            delete postbox[_key]
                        }

                    }
                    catch (e) {
                        console.log(e)
                    }

                }, 10000)

                postbox[devices[_deviceid].address + ":" + devices[_deviceid].port] = (message) => {
                    clearTimeout(hTimer)
                    res.json({ r: 'ok', msg: message.toString() })
                }

                const _key = devices[_deviceid].address + ":" + devices[_deviceid].port

                nuts_udp_server.send(_msg,
                    devices[_deviceid].port,
                    devices[_deviceid].address, (err) => {

                        if (err) {
                            console.log(err)
                            if (postbox[_key]) {
                                res.json({ r: 'err', msg: err.toString() })
                                delete postbox[_key]
                            }
                        }
                        else {
                            console.log('send success ok')
                        }
                    });

            }
            catch (e) {
                // res.json({ r: 'err', msg: e.toString() })
                return
            }



        }
        else {
            res.json({ r: 'err', msg: 'device not found' })
        }
    }
    );


    function _update() {

        //타이오버된 디바이스 정보 삭제
        let _now = new Date().getTime()
        for (let key in devices) {
            if (_now - devices[key].at > 30000) {
                console.log(`device ${key} is out of time`)
                delete devices[key]
            }
        }
        setTimeout(_update, 5000)
    }

    //디바이스 정보 업데이트
    _update();

    return router;

}
