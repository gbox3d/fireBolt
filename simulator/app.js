/**
 * Created by gbox3d on 2014. 1. 25..
 */


var theApp = {

    config: {
        base_url : 'http://' ///sparkDeviceCommandCheck'
    },
    timer : new gbox3d.core.Timer(),
    deviceID : '',
    deviceStatus : 'ready',
    UIElements : {}
};

theApp.UIElements.ledNetwork = document.querySelector('#led-network');



//명령어해석하기
function decodeCommand(code) {

    var operator = code.substr(1,2);

    console.log(operator);

    switch (operator) {
        case '01': //디지털쓰기

            (function() {

                var port_number = code.substr(3,2);
                var value = code.substr(5,3);

                console.log(port_number + ',' + value );

                var led = document.querySelector('#part-led .led-port'+port_number);

                if(value > 0) {
                    led.classList.remove('simeffect-led-off');
                    led.classList.add('simeffect-led-on');

                }
                else {
                    led.classList.remove('simeffect-led-on');
                    led.classList.add('simeffect-led-off');
                }


            })();


            break;
        case '02':
            break;
        default :
            break;
    }



}

function fetchCommand(command) {

    $.ajax(
        {
            url: theApp.config.base_url + '/sparkDeviceCommandExec',
            type: 'GET',
            jsonp: 'callback',
            jsonpCallback: 'callback',
            dataType: 'jsonp',
            data: {
                id : theApp.deviceID,
                commandID : command.id

            },
            timeout : 3000,
            success: function(data, textStatus, jqXHR) {
                console.log(data);


                //{ result: 'saveSuccess', commandID: 'C00036' }

                if(data.result == 'saveSuccess') {

                    decodeCommand(command.code);

                }

            },
            error: function(jqXHR, textStatus) {
                console.log(textStatus);
            },
            complete: function(jqXHR, textStatus, errorThrown) {

                console.log('fetch complete : ');
                //다음턴 준비
                //setTimeout(device_loop,1000);

            }
        }
    );


}

function device_loop() {

    //console.log('connecting... ');
    //theApp.UIElements.ledNetwork.style.backgroundColor = 'red';

    theApp.timer.getDeltaTime(); //타이머 리셋..
    $.ajax(
        {
            url: theApp.config.base_url + '/sparkDeviceCommandCheck',
            type: 'GET',
            jsonp: 'callback',
            jsonpCallback: 'callback',
            dataType: 'jsonp',
            data: {
                id : theApp.deviceID
            },
            timeout : 3000,
            success: function(data, textStatus, jqXHR) {

                console.log(data);

                if(data.result == 'LoadSuccess') {

                    fetchCommand({
                        id : data.item[0].commandID,
                        code : data.item[0].commandCode
                    });
                }

            },
            error: function(jqXHR, textStatus) {
                console.log(textStatus);
                theApp.UIElements.ledNetwork.style.backgroundColor = 'red';
            },
            complete: function(jqXHR, textStatus, errorThrown) {

                //console.log('complete : ' + theApp.timer.getDeltaTime());
                //console.log(theApp.UIElements.ledNetwork.style.backgroundColor);

                if(theApp.UIElements.ledNetwork.style.backgroundColor == 'green') {
                    theApp.UIElements.ledNetwork.style.backgroundColor = 'black';
                }
                else {
                    theApp.UIElements.ledNetwork.style.backgroundColor = 'green';
                }


                //다음턴 준비
                if(theApp.deviceStatus == 'run') {
                    setTimeout(device_loop,1000);
                }
                else {//루프 종료
                    theApp.UIElements.ledNetwork.style.backgroundColor = 'black';
                }


            }
        }
    );


}