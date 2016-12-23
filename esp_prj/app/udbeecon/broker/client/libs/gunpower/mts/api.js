/**
 * Created by gbox3d on 2014. 8. 1..
 */

gunpower.mts.api = function(option) {

    var url = option.url;
    var saro_number =   option.saro_number ? parseInt(option.saro_number) : 0 ;

    var super_self = this;

    this.target_cmd_flags = [];
    this.target_hit_tick = [];

    var conn_type = option.conn_type != undefined ? option.conn_type : 'http';

    console.log('controler type : ' + conn_type);

    if(conn_type == 'bt') {

        this.resumeTargetControl = function() {

        }

        this.stopTargetControl = function() {

        }

        this.directCommand = function(option) {
            var code = JSON.parse('[' + option.param.code + ']' );
            gts_v1.sendData(
                JSON.stringify({
                    type : 'bt',
                    code : code
                })
            );

            option.callback({
                result : 'ok'
            });
            console.log(code + ' excute!');
        }
        //---------//

    }
    else if(conn_type == 'http') {
        this.directCommand = function(option) {

            option.param.saro = saro_number;

            $.ajax({
                url : url + '/' +option.cmd,
                type : "GET",
                dataType : "text",
                data : option.param,
                timeout: 10000,
                success : function(data,textStatus,jqXHR) {

                    //console.log(data);
                    var _data = JSON.parse(data);
                    option.callback(_data);

                },
                complete : function() {

                },
                error : function(jqXHR, textStatus, errorThrown) {

                    option.callback({
                        result : 'ajax_error',
                        error : textStatus
                    })
                }

            });

        }
    }
    else if(conn_type == 'udp') {

        this.directCommand = function(option) {

            var udp_beacon = theApp.systemInfo.udp_beacon;
            var code = JSON.parse('[' + option.param.code + ']' );

            console.log(code);

            var tbl = [2,4,8,16,32,64]

            var stand_value = 0;
            var stand_values = [];
            var fall_value = [];
            var moving = 0;
            for(var i=0;i<code.length;i++) {
                if(85 == code[i]) { //U
                    var index = code[i-1]-48;
                    stand_value += tbl[index]; //stand 용
                    stand_values.push(index); //up 용

                    //this.target_cmd_flags[index] = 30;

                }
                if(68 == code[i]) { //D
                    fall_value.push(code[i-1]-48)
                }
                if(77 <= code[i] && 83 >= code[i] || 109 == code[i]) {
                    if(code[i] != 81) {
                        moving = [code[i]]
                    }

                }

            }

            var packet_set = ""

            //무빙제어
            if(moving > 0) {

                var packet = {
                    C:String.fromCharCode(moving),
                    I:10
                }

                packet_set +=  JSON.stringify(packet)
                /*
                gts_v1.sendUDPData(JSON.stringify(
                    {
                        ip: udp_beacon.ip,
                        port:udp_beacon.port,
                        data : JSON.stringify(packet)
                    }
                ))
                console.log(packet);
                */

            }

            //세우기
            if(stand_value > 0) {

                if(theApp.mode == 'test') {

                    if(stand_value == 126) { //모두 일어설경우

                        var packet = {
                            C:'U',
                            I:255
                        }

                        packet_set +=JSON.stringify(packet)
                        /*
                        gts_v1.sendUDPData(JSON.stringify(
                            {
                                ip: udp_beacon.ip,
                                port:udp_beacon.port,
                                data : JSON.stringify(packet)
                            }
                        ))
                        */
                        console.log(packet);

                    }
                    else {

                        for(var i=0;i<stand_values.length;i++) {
                            var packet = {
                                C:'U',
                                I:stand_values[i]+1
                            }

                            console.log('test mode');
                            console.log(packet);

                            /*
                            gts_v1.sendUDPData(JSON.stringify(
                                {
                                    ip: udp_beacon.ip,
                                    port:udp_beacon.port,
                                    data : JSON.stringify(packet)
                                }
                            ));
                            */
                            packet_set +=  JSON.stringify(packet)
                        }
                    }

                }
                else {

                    var packet = {
                        C:'S',
                        I:255,
                        V:stand_value
                    }
                    /*
                    gts_v1.sendUDPData(JSON.stringify(
                        {
                            ip: udp_beacon.ip,
                            port:udp_beacon.port,
                            data : JSON.stringify(packet)
                        }
                    ))
                    */
                    packet_set +=  JSON.stringify(packet)
                    console.log(packet);

                }


            }

            //넘어뜨리기
            if(fall_value.length > 0 ) {
                //전체 넘어뜨리기
                if( fall_value.length >= 6) {

                    var packet = {
                        C:'D',
                        I:255
                    }
                    /*
                    gts_v1.sendUDPData(JSON.stringify(
                        {
                            ip: udp_beacon.ip,
                            port:udp_beacon.port,
                            data : JSON.stringify(packet)
                        }
                    ));
                    */
                    packet_set +=  JSON.stringify(packet)
                    console.log('clear all');
                    console.log(packet);

                }
                else {

                    for(var i=0;i<fall_value.length;i++) {
                        var packet = {
                            C:'D',
                            I:fall_value[i]+1
                        }
                        /*
                        gts_v1.sendUDPData(JSON.stringify(
                            {
                                ip: udp_beacon.ip,
                                port:udp_beacon.port,
                                data : JSON.stringify(packet)
                            }
                        ));
                        */
                        packet_set +=  JSON.stringify(packet)
                        console.log(packet);
                    }

                }

            }

            console.log(packet_set)
            gts_v1.sendUDPData(JSON.stringify(
                {
                    ip: udp_beacon.ip,
                    port:udp_beacon.port,
                    data : packet_set
                }
            ));

            option.callback({
                result : 'ok'
            });
            //console.log(code + ' excute!');


        }
        function directCommand_udp(packet) {
            var udp_beacon = theApp.systemInfo.udp_beacon;
            /*
            var packet = {
                C:'S',
                I:255,
                V:2
            }
            */
            console.log(JSON.stringify(packet))
            gts_v1.sendUDPData(JSON.stringify(
                {
                    ip: udp_beacon.ip,
                    port:udp_beacon.port,
                    data : JSON.stringify(packet)
                }
            ))

        }

        this.resumeTargetControl = function() {

            directCommand_udp({C:'R'})

        }

        this.stopTargetControl = function() {
            directCommand_udp({C:'T'})
        }

    }
    else {
        this.directCommand = function(option) {
            console.log("no connection type defined!");
        }

    }


    this.getLineNumber = function() {
        return saro_number;
    }

}


gunpower.mts.server_api =  {
    msg_api : {
        url : 'gunpower-rd.iptime.org',//'192.168.0.189',
        port : 1883,
        method : {
            select_log : function(evt) {

                var context = gunpower.mts.server_api.msg_api;
                var return_data;

                var str_where = "memo like '%\"shop_id\":\"" + evt.shop_id +"\"%' AND " +
                    "memo like '%\"shoot_line\":"+ evt.shoot_line + "%'"

                $.ajax(
                    {
                        url :'http://' + context.url + ':' + context.port + '/select-log',
                        data : {
                            where : str_where},
                        dataType:'text',
                        timeout : 3000,
                        success : function (data,textstatus,xhr) {
                            //console.log(data);
                            //console.log('save success');
                            return_data = data;
                        },
                        error : function(xhr,textstatus) {
                            console.log(textstatus);

                        },
                        complete : function(xhr,textstatus,err) {

                            if(err) {
                                evt.callback({result:'err'});

                            }
                            else {
                                evt.callback(return_data);
                            }
                        }
                    }
                );
                //---ajax

            }
            //--------------//
        }

    },
    db_api : {
        url : 'gunpower-rd.iptime.org',
        port : 8081,
        method : {
            setup : function() {

                var db_context = gunpower.mts.server_api.db_api;
                db_context.miss_log = [];

                gts_v1.OnReadFileComplete = function(data) {

                    //console.log(data);
                    db_context.miss_log = JSON.parse(data);
                    console.log('success load miss_log ' + db_context.miss_log.length);

                }
                gts_v1.OnReadFileNotfound = function() {

                    db_context.miss_log = [];
                    console.log('miss log not found');

                }

                //못올린 로그데이터  불러오기
                gts_v1.readFile(JSON.stringify({filename : '/sdcard/GTS815/miss_log.json'}));


            },
            restore_misslog : function(option) {
                /*
                 todo .... 데이터를 하나씩 복구할필요가 있음
                 일괄적으로 업로딩하다 실패했을때 처리가 필요함. 데이터가 중복 될수있음
                 */

                var db_context = gunpower.mts.server_api.db_api;

                function _pushlog(option) {

                    var send_data = {
                        card_index : theApp.player_card_index,
                        shop_id : theApp.systemInfo.shop_id,
                        point : option.memo.point,
                        time : option.memo.time,
                        memo : JSON.stringify(option.memo)
                    };


                    /*
                    console.log(send_data);

                    if(option.complete) {
                        option.complete();
                    }
                    */


                    $.ajax(
                        {
                            url :'http://' + db_context.url + ':' + db_context.port + '/push-log-2',
                            data : send_data,
                            dataType:'text',
                            timeout : 3000,
                            success : function (data,textstatus,xhr) {
                                console.log(data);
                                console.log('save success');
                                if( option.success ) {
                                    option.success(data);
                                }

                            },
                            error : function(xhr,textstatus) {
                                console.log(textstatus);
                                if(option.error) {
                                    option.error(textstatus);
                                }
                                //--------
                            },
                            complete : function(xhr,textstatus,err) {
                                if(option.complete) {
                                    option.complete(textstatus);
                                }

                            }
                        }
                    );

                    
                }
                
                var index = 0;
                function loop() {
                    if(index < db_context.miss_log.length) {

                        _pushlog({
                            // point : db_context.miss_log[index].point,
                            memo : db_context.miss_log[index],
                            success : function(data) {
                                //console.log('success :' + index);
                                
                                if(option.onProgress) {
                                    option.onProgress(index);
                                }
                                
                            },
                            error : function() {
                                //console.log('error');
                            },
                            complete : function() { //완료
                                index++;
                                setTimeout(loop,300);

                            }
                        });
                        //--------
                    }
                    else {
                        //복구 롼료 되었다면
                        console.log('complete backup  : ' + index);
                        //완료
                        db_context.miss_log = [];

                        gts_v1.OnWriteFileComplete = function(data) {
                            console.log(data);
                            //작업완료..
                            if(option.callback) {
                                option.callback();
                            }
                            
                        }
                        gts_v1.writeFile(
                            JSON.stringify({
                                filename : '/sdcard/GTS815/miss_log.json',
                                data : JSON.stringify(db_context.miss_log)
                            })
                        );

                    }
                }
                loop();
                
            },
            push_log : function(option) {

                var db_context = gunpower.mts.server_api.db_api;

                var send_data = {
                    card_index : theApp.player_card_index,
                    shop_id : theApp.systemInfo.shop_id,
                    point : option.game_play.score

                };

                var memo =  {
                    card_number : theApp.player_card_number,
                    point     : option.game_play.score,
                    play_time : option.game_play.play_time,
                    level     : option.game_play.current_level,
                    saro   : theApp.systemInfo.saro,
                    bt_mac : theApp.systemInfo.bt_mac
                }

                if(option.debug_log) {
                    memo.debug_log = option.debug_log
                }

                //플레이하지않은표시, 플레이 타임이 없는걸로 나옴
                if(option.game_play.play_time == 0) {
                    delete memo['play_time']
                    delete memo['point']
                    delete memo['level']

                }


                send_data.memo = JSON.stringify(memo);

                console.log(send_data)

                $.ajax(
                    {
                        url :'http://' + db_context.url + ':' + db_context.port + '/push-log',
                        data : send_data,
                        dataType:'text',
                        timeout : 3000,
                        success : function (data,textstatus,xhr) {
                            console.log(data);
                            console.log('save success');
                            if( option.success ) {
                                option.success(data);
                            }

                        },
                        error : function(xhr,textstatus) {
                            console.log(textstatus);

                            db_context.miss_log.push({
                                card_number : theApp.player_card_number,
                                point     : option.game_play.score,
                                play_time : option.game_play.play_time,
                                level     : option.game_play.current_level,
                                saro   : theApp.systemInfo.saro,
                                bt_mac : theApp.systemInfo.bt_mac,
                                card_index : theApp.player_card_index,
                                shop_id : theApp.systemInfo.shop_id,
                                point : option.game_play.score,
                                time : new Date().getTime()
                            });

                            gts_v1.OnWriteFileComplete = function(data) {
                                console.log(data);
                            }

                            gts_v1.writeFile(
                                JSON.stringify({
                                    filename : '/sdcard/GTS815/miss_log.json',
                                    data : JSON.stringify(db_context.miss_log)
                                })
                            );
                            //-

                        },
                        complete : function(xhr,textstatus,err) {

                        }
                    }
                );
                //--
            },
            push_log2 : function(option) {

                var db_context = gunpower.mts.server_api.db_api;

                var send_data = {
                    card_index : option.card_index,
                    shop_id : option.shop_id,
                    point : option.point,
                    memo : JSON.stringify(option.memo)
                };

                $.ajax(
                    {
                        url :'http://' + db_context.url + ':' + db_context.port + '/push-log',
                        data : send_data,
                        dataType:'text',
                        timeout : 3000,
                        success : function (data,textstatus,xhr) {
                            console.log(data);
                            console.log('save success');
                            if( option.success ) {
                                option.success(data);
                            }

                        },
                        error : function(xhr,textstatus) {
                            console.log(textstatus);

                            db_context.miss_log.push( JSON.parse(send_data.memo) );

                            gts_v1.OnWriteFileComplete = function(data) {
                                console.log(data);
                            }

                            gts_v1.writeFile(
                                JSON.stringify({
                                    filename : '/sdcard/GTS815/miss_log.json',
                                    data : JSON.stringify(db_context.miss_log)
                                })
                            );
                            //-

                        },
                        complete : function(xhr,textstatus,err) {

                        }
                    }
                );
                //--
            },
            //--------------------//
            check_id : function(option) {

                console.log(option);

                $.ajax({
                    url :'http://' + gunpower.mts.server_api.db_api.url + ':' + gunpower.mts.server_api.db_api.port +  '/get-shop-info',
                    data : {id: option.id, passwd : option.passwd},

                    dataType : 'text',
                    timeout : 5000,
                    success : function(data,status,xhr) {
                        option.callback({
                            result:'ok',
                            data : data
                        })
                        //next(null,JSON.parse(data));
                    },
                    error : function(xhr,status) {
                        //next(status);
                        option.callback({
                            result:'err',
                            err : status
                        })
                    }
                });

            },
            //------------------//
            get_log : function(option) {
                console.log(option);

                //gunpower-rd.iptime.org:8081/select-log?where=shop_id=%27test1%27

                $.ajax({
                    url :'http://' + gunpower.mts.server_api.db_api.url + ':' + gunpower.mts.server_api.db_api.port +  '/select-log',
                    data : {
                        where : "shop_id='" + option.shop_id  + "' AND memo like '%\"card_number\":" + option.card_number + "%'"
                    },
                    dataType : 'text',
                    timeout : 5000,
                    success : function(data,status,xhr) {
                        option.callback( JSON.parse(data))
                        //next(null,JSON.parse(data));
                    },
                    error : function(xhr,status) {
                        //next(status);
                        option.callback({
                            result:'err',
                            err : status
                        })
                    }
                });

            },
            //----------------//
            //http://gunpower-rd.iptime.org:8081/get-card-info?card_number=7516015233300862
            get_card_info : function(option) {
                $.ajax(
                    {
                        url :'http://' + gunpower.mts.server_api.db_api.url + ':' + gunpower.mts.server_api.db_api.port + '/get-card-info',
                        data : {
                            card_number : option.card_number
                        },
                        dataType:'text',
                        timeout : 3000,
                        success : function (data,textstatus,xhr) {
                            console.log(data);

                            option.callback(JSON.parse(data));

                        },
                        error : function(xhr,textstatus) {
                            console.log(textstatus);
                            option.callback({
                                result:'err',
                                err : status
                            });

                        },
                        complete : function(xhr,textstatus,err) {

                        }
                    }
                );
                //---------
            }
            //--------------------//
        }
    },
    login_api : {
        url : 'gunpower-rd.iptime.org',
        port : 8081,
        method : {


        }
    },
    udp : {

    }
}
