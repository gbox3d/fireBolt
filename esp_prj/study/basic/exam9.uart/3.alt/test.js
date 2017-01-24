/**
 * Created by gbox3d on 15. 7. 20..
 */

var serialport = require("/usr/local/lib/node_modules/serialport");
var baudrate = 115200;

//ls /dev | grep usb
var serialPort = new serialport.SerialPort(
    "/dev/tty.usbserial",
    {
        baudrate: baudrate,
        parser: serialport.parsers.readline("\r\n") //개행문자기준으로 마샬링해주기
    }
);

serialPort.on("open", function () {
    console.log('open at baudrate :' + baudrate);

    setInterval(function() {
        console.log('send data');
        //var buf = Buffer([0x01,0x02,0x03,0x8,0x81]);
        var buf = Buffer([0x41,0x42,0x43,0x44,0x0d]);
        serialPort.write(buf);
        console.log(buf);

    },3000);

    //데이터 읽기
    serialPort.on('data', function(data) {

        console.log('rev data :');
        console.log(data );
    });

});
