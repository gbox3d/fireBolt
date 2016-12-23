/**
 * Created by gbox3d on 15. 8. 28..
 */

//네이티브 시뮬레이터 생성
gunpower.mts.nativeSimulator = function() {

    var context = this;

    this.setSoundEffect = function() {}
    this.PlayWav = function() {}
    this.startMag = function() {}
    this.stopMag = function() {}
    this.getSystemInfo = function() {
        setTimeout(function() {

            /*
             theApp.systemInfo = {};
             //시스템 기본값
             theApp.systemInfo.shop_id = 'test1';
             theApp.systemInfo.bt_mac = '00:00:00:00';
             theApp.systemInfo.saro = 0;
             */

            if(localStorage.t250_systeminfo) {
                context.OnGetSystemInfo(
                    JSON.parse(localStorage.t250_systeminfo)
                );
            }
            else {
                context.OnGetSystemInfo(
                    {}
                );
            }
            //-

        },500)

    }
    //---------------//
    this.setSystemInfo = function( strJson_sysinfo ) {

        localStorage.t250_systeminfo = strJson_sysinfo;

    };
    //---------------//
    this.connectBt = function() {

        //블루투스 에서 데이타 전달콜백
        //gts_v1.OnBTReadData

        setTimeout(function() {

            context.OnBTConnected({
                device_name : 'simulator'
            })

        },1000);
    }

    //---------------//
    this.sendData = function(arg) {

        console.log(arg);
    }

    //---------------//
    this.ProcessMp3 = function(arg) {
        console.log('native call ProcessMp3');
        console.log(arg);

        if(JSON.parse(arg).action == 'stop') {

            console.log('stop action');
        }

    }
    //-----------------//
    this.readFile = function(option) {

    }
    this.writeFile = function(option) {

    }
    this.getFileList = function(option) {
        gts_v1.OnGetFileList('{"file_list":["lv6.mp3","lv4.mp3","lv3.mp3","lv2.mp3","lv1.mp3","city_1.wav","run_n_gun.wav"]}');
    }
    this.getFxSoundList = function(option) {
        gts_v1.OnGetFxSoundList('{"hit11":11,"citywar_fx5":18,"hijack_fx4":22,"hit10":10,"citywar_fx4":17}');
    }

}
