const dgram = require("dgram");
const port = 50020;

const server = dgram.createSocket("udp4");
const networkInterfaces = require('os').networkInterfaces

//bis 형식 날짜,시간 숫자 구하기
function getBisDateNumber(d) {
    return d.getFullYear() * 10000 + (d.getMonth() + 1) * 100 + d.getDate()

}

function getBisTimeNumber(d) {
    return d.getHours() * 10000 + d.getMinutes() * 100 + d.getSeconds()

}

function getLocalIp() {
    let localIp = Object.entries(networkInterfaces())
        .map(o => [o[0], o[1].filter(q => q.family === 'IPv4' && !q.internal).map(r => r && r.address)]).filter(([a, b]) => b.length > 0)
        .map(([a, b]) => b[0])[0];

    return localIp;
}


server.on("message", function (msg, rinfo) {

    console.log(rinfo);
    console.log(msg);

    let _offset = 0;
    let _stx = msg.readUInt8(_offset++)
    let _code = msg.readUInt8(_offset++)
    let _sr_cnt = msg.readUInt16LE(_offset)
    _offset += 2
    let _id = msg.readUInt32LE(_offset)
    _offset += 4
    let _data_size = msg.readUInt32LE(_offset)
    _offset += 4

    console.log(`code : ${_code}, id : ${_id} ,sr_cnt : ${_sr_cnt} ,data size : ${_data_size}`)

    let resBuf = new Buffer.alloc(128)
    _offset = 0
    switch (_code) {
        case 0x05: //ping
            {
                resBuf.writeUInt8(0x02, _offset++) // stx
                resBuf.writeUInt8(0x01, _offset++) //op code ack
                resBuf.writeUInt16LE(_sr_cnt + 1, _offset) //sr_cnt
                _offset += 2
                resBuf.writeUInt32LE(_id, _offset)
                _offset += 4
                resBuf.writeUInt32LE(12, _offset) //data size
                _offset += 4

                //body
                let _date = new Date()
                resBuf.writeUInt32LE(getBisDateNumber(_date), _offset)
                _offset += 4
                resBuf.writeUInt32LE(getBisTimeNumber(_date), _offset)
                _offset += 4
                resBuf.writeUInt8(_code, _offset++) //대상 opcode
                _offset += 3 //예약 영역
                
                resBuf.writeUInt8(0, _offset++) //check sum
                resBuf.writeUInt8(0x03, _offset++) //etx
            }
            break;
        default:
            {
                resBuf.writeUInt8(0x02, _offset++) // stx
                resBuf.writeUInt8(0x01, _offset++) //op code ack
                resBuf.writeUInt16LE(_sr_cnt + 1, _offset) //sr_cnt
                _offset += 2
                resBuf.writeUInt32LE(_id, _offset)
                _offset += 4
                resBuf.writeUInt32LE(12, _offset) //data size
                _offset += 4

                //body
                let _date = new Date()
                resBuf.writeUInt32LE(getBisDateNumber(_date), _offset)
                _offset += 4
                resBuf.writeUInt32LE(getBisTimeNumber(_date), _offset)
                _offset += 4
                resBuf.writeUInt8(_code, _offset++)
                resBuf.writeUInt8(0xff, _offset++) //error 원인,  0xff 는 존재 하지않는 code
                _offset += 2 //예약 영역

                resBuf.writeUInt8(0, _offset++) //check sum
                resBuf.writeUInt8(0x03, _offset++) //etx


            }
            break;
    }

    server.send(
        resBuf, 0, _offset,
        rinfo.port, rinfo.address); // added missing bracket
});

server.bind(port);

console.log(`udp server bind at ${getLocalIp()} : ${port}`)
