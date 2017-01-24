/**
 * Created by gunpower on 2016. 1. 13..
 */

(function (){

    function RemoteManager(option) {

        var loop_delay = option.loop_delay;//1000;
        var base_url = option.base_url;//'http://gunpower-rd.iptime.org:3389'
        var id = option.id;
        var isStop = false;

        console.log('remote manager setup...');
        console.log(option)


        function loop() {

            $.ajax({
                url: base_url + '/shift',
                type:'GET',
                dataType : 'text',
                timeout : 5000,
                data : {id : id },
                success : function(data,text,xhr) {

                    var data1 = JSON.parse(data);
                    if(data1.result == 'ok') {
                        var data2 = JSON.parse(data1.content);
                        if(option.onReceviveData) {
                            option.onReceviveData(data2)
                        }

                    }

                    //console.log(data);

                },
                error : function() {

                },
                complete : function() {
                    if(loop_delay > 0 && !isStop ) {
                        setTimeout(function() {
                            loop();

                        },loop_delay)

                    }
                    else {
                        console.log('loop stop');
                    }

                }

            })
        }

        this.start = function(option) {
            isStop = false;

            if(option.reset) {

                //오프라인상태일때 못받은 값들은 지워버린다
                $.ajax({
                    url: base_url + '/clear',
                    type:'GET',
                    dataType : 'text',
                    timeout : 5000,
                    data : {id : id },
                    success : function(data,text,xhr) {
                        loop();

                    },
                    error : function() {

                    },
                    complete : function(xhr,status,error) {
                        if(option.callback) {
                            option.callback(status);
                        }

                    }

                })

            }
            else {
                loop();
                if(option.callback) {
                    option.callback();
                }
            }



        }
        this.stop = function() {
            isStop = true;
        }
        this.set_loopDelay = function(delay) {
            loop_delay = delay;
        }
        this.send = function(data,callback) {
            //http://gunpower-rd.iptime.org:3389/push?to_id=test1@1&content={"fn":"setdio","pin":0,"val":1,"from":"tsmanager"}
            $.ajax({
                url: base_url + '/push',
                type:'GET',
                dataType : 'text',
                timeout : 5000,
                data : {to_id : 'tsmanager',content : data },
                success : function(data,text,xhr) {
                    console.log(data);
                },
                error : function() {

                },
                complete : function(xhr,status,error) {
                    callback(status)

                }

            })

        }


    }

    gunpower.mts.RemoteManager = RemoteManager;

})()
