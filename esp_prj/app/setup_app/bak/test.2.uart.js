/**
 * Created by gbox3d on 15. 6. 19..
 */


var serialport = require("/usr/local/lib/node_modules/serialport");
var baudrate = 115200;

//ls /dev | grep usb
var serialPort = new serialport.SerialPort(
    "/dev/tty.usbserial17",
    {
        baudrate: baudrate
        //parser: serialport.parsers.readline("\r\n") //개행문자기준으로 마샬링해주기
    }
);

serialPort.on("open", function () {

    console.log('open at baudrate :' + baudrate);

    //
    //setTimeout(function() {
    //
    //    serialPort.write('!0113000.',function() {});
    //
    //},3000);
    //
    //데이터 읽기
    serialPort.on('data', function(data) {
        console.log(data );


    });

});