/**
 * Created by gbox3d on 2014. 8. 1..
 */

//효과음
//theApp.method.sound.playFX('double_kill');

gunpower.mts.game = function(option) {

    var controller = option.controller;

    var fsm = 'boot-up';
    var fsm_step = 0;

    function setFSM(new_fsm) {

        fsm = new_fsm;
        fsm_step = 0;
    }

    var game_play = {
        target_status : [0,0,0,0,0,0],
        play_target : [],
        current_level : 0,
        turn_count : 0,
        score : 0,
        play_time : 0,
        level_def : option.level_def,
        combo : {
            combo_tick_limit : 0, //유효시간이 0이면 콤보가 없는것이겟지요?
            hit_tick_start : 0, //틱카운트 시작기준
            combo_count : 0,
            combo_bonus : [],
            //fx_snd : ['double_kill','comboSound','comboSound','niceshot','niceshot']
            fx_snd : ['comboSound','comboSound','comboSound','comboSound','comboSound']
        }
    };

//////////랜덤 모듈
    function random_mts(option) {

        var table = option.table;

        var index = 0;

        var shuffle = function() {

            for(var i=0 ;i<table.length;i++) {

                var swap = gbox3d.core.randomIntFromTo(0,table.length-1);

                var temp = table[i];

                table[i] = table[swap];

                table[swap] = temp;
            }

            //console.log('random shuffle...');
            //console.log(table);

        }

        var enable_shuffle = true;
        var reserve_shffle = false; //셔플예약

        //셔플중지
        this.no_shuffle = function() {
            enable_shuffle = false;
        }

        //셔플 가능
        this.yes_shuffle = function() {
            enable_shuffle = true;

            if(reserve_shffle) {
                reserve_shffle = false;
                shuffle();
            }

        }
        this.getTableSize = function() {
            return table.length;
        }

        this.get = function() {

            if(index >= table.length) {

                if(enable_shuffle ) {
                    shuffle();
                    reserve_shffle = false;
                }
                else {
                    reserve_shffle = true;
                }

                index = 0;
            }
            return table[index++];
        }

        shuffle();

    }


    ///랜덤 초기화
    ////////////////////
    var random_tanble = [0,1,2,3,4,5];

    //고장난타겟체크해서 랜덤에 반영시키기
    for(var i=0;i<theApp.game_def.device_def.disable_unit.length;i++) {

        var exclude_index = theApp.game_def.device_def.disable_unit[i];

        random_tanble.splice( random_tanble.indexOf(exclude_index),1);

    }

    //콤보정의
    if(theApp.game_def.combo) {
        game_play.combo.combo_bonus = theApp.game_def.combo.bonus;
        game_play.combo.combo_tick_limit = theApp.game_def.combo.limit_tick;
    }
    else {
        game_play.combo.combo_bonus = [5,10,15,20,30];
        game_play.combo.combo_tick_limit = 2000;
    }

    var random_obj = new random_mts({
        table : random_tanble//[0,1,2,3,4,5]
    });

    /////////////////////
    //내부 함수
    ////////////////////

    //고장난 타겟인지 검사
    function isDisableUnit(target_index) {

        for(var i=0;i<theApp.game_def.device_def.disable_unit.length;i++) {

            if( target_index == theApp.game_def.device_def.disable_unit[i] ) {
                return true;
            }
        }

        return false;

    }

    //타겟이 모두내려간상태인지 검사
    function isAllTargetSleep() {
        for(var i=0;i<currentTarget_Status.length;i++) {

            if(currentTarget_Status[i] != 68 && (currentTarget_Status[i] != 0)
                && (isDisableUnit(i) == false) ) {
                return false;
            }
        }

        return true;

    }

    function clearAllTarget(callback) {

        //모든타겟정리
        //console.log('try clear all target');

        async.waterfall(
            [
                function(next) {
                    //모든효과 트리거 제거
                    next(null);
                },
                function(next) {

                    //
                    var code = '48,68,48,109,49,68,50,68,51,68,52,68,53,68';
                    //무빙제외
                    //var code = '48,68,49,68,50,68,51,68,52,68,53,68';

                    controller.directCommand({
                        cmd : 'control',
                        param : {
                            code : code
                        },
                        callback : function(evt) {
                           // console.log(evt);
                            next(null,{
                                result : 'ok',
                                msg : 'all target clear'
                            });

                        }
                    });
                }
            ],
            function(err,result) {

                for(var i=0;i<game_play.target_status.length;i++) {
                    game_play.target_status[i] = 0;
                }

                callback(result);
            }
        );
    }




    //////////////////////////
//외부 접근 함수
    /////////////////////////////


    var context = this;

    //타이머핸들러
    var wait_alldown_timer_id = -1;
    var wait_all_stand_timer_id = -1;

    this.getFsm = function() {
        return fsm;
    }

    this.reset = function() {
        fsm = 'end-game';
        fsm_step = 0;

        //타이머 핸들러 정리
        if(wait_alldown_timer_id >= 0) {
            console.log('clear wait_alldown_timer :' + wait_alldown_timer_id);
            clearTimeout( wait_alldown_timer_id);
            wait_alldown_timer_id = -1;
        }
        if(wait_all_stand_timer_id >= 0) {
            console.log('clear timer wait_all_stand_timer : ' + wait_all_stand_timer_id)
            clearTimeout(wait_all_stand_timer_id);
            wait_all_stand_timer_id = -1;

        }
    }
    /////////////////////////////
    var hit_sound_effect_table =[]
    var deltaTime = 0;
    var prevTime = 0;
    var currentTime = 0;
    var currentTarget_Status;

    var loop = function() {

        currentTime = (new Date).getTime();
        deltaTime = currentTime - prevTime;

        if(option.loopCallback) {
            option.loopCallback({
                deltaTime : deltaTime,
                currentTime : currentTime,
                FSM : fsm,
                gameplay : game_play

            });
        }

        //console.log(game_play.play_time );
        //console.log(fsm + ',' + fsm_step);
        switch (fsm) {
            case 'boot-up':
                setFSM('pre-ready');
                break;
            case 'pre-ready':
                (function() {
                    if(fsm_step == 0) {
                        async.waterfall(
                            [
                                function(next) {
                                    code = '48,85,49,85,50,85,51,85,52,85,53,85';
                                    controller.directCommand({
                                        cmd : 'control',
                                        param : {
                                            code : code
                                        },
                                        callback : function(evt) {
                                            //console.log(evt);
                                            next(null);
                                        }
                                    });
                                }
                            ],
                            function(error,result) {

                                setFSM('ready');
                            }
                        );
                        fsm_step++;
                    }
                    else {

                    }

                })();
                //////////////////////pre-ready end
                break;
            case 'ready':
                (function() {
                    if(fsm_step == 0) {

                        fsm_step++;

                        ///////////////callback
                        if(option.readyCallback) {
                            option.readyCallback({});
                        }
                        ///////////////////////

                        //효과음 오프
                        //gts_v1.setSoundEffect('mute');
                        theApp.method.sound.mute(true);
                        theApp.event_listener.OnHit = null;
                        //시작 감지...
                        theApp.event_listener.OnStartGame = function (evt) {

                            //효과음 온!
                            theApp.method.sound.mute(false);
                            //gts_v1.setSoundEffect('on');

                            setFSM('start-game');
                            theApp.event_listener.OnStartGame = null;
                            //fsm = 'start-game';
                            //fsm_step = 0;

                        }


                        theApp.OnDeskBtnCallback = function() {

                            console.log('OnDeskBtnCallback start');

                            theApp.method.proc_gamestart();
                            theApp.OnDeskBtnCallback = null;

                            setTimeout(function() {

                                console.log('OnDeskBtnCallback playing setup');

                                //게임종료 처리
                                theApp.OnDeskBtnCallback = function() {

                                    console.log('OnDeskBtnCallback stop');

                                    theApp.gameObject.reset();
                                    theApp.method.proc_gameover();
                                    //theApp.object.score_panel.game_over_Action();
                                    theApp.OnDeskBtnCallback = null;
                                }
                            },1000);
                        }



                    }
                    else if(fsm_step == 1) {

                        //무빙정지보내기
                        controller.directCommand({
                            cmd : 'control',
                            param : {
                                code : '48,109'
                            },
                            callback : function(evt) {
                                //console.log(evt);
                            }
                        });

                        fsm_step = 2;
                    }
                    else {

                    }

                })();
                /////////////////////ready end
                break;
            case 'start-game':

                (function() {
                    if(fsm_step == 0) {
                        fsm_step++;
                        game_play.current_level = 0;
                        game_play.play_target = [];
                        game_play.score = 0;
                        game_play.play_time = 0;

                        //넘기지말고 시작
                        /*
                        setTimeout(function() {

                            setFSM('start-level');
                            if(option.gameStartCallBack) {
                                option.gameStartCallBack(
                                    {
                                        game_play : game_play
                                    }
                                )
                            }

                        },100);
                        */
                        //-------------------//


                        clearAllTarget(function(evt) {
                            if(evt.result == 'ok') {
                                setFSM('start-level');
                                if(option.gameStartCallBack) {
                                    option.gameStartCallBack(
                                        {
                                            game_play : game_play
                                        }
                                    )
                                }
                            }
                            else {
                                setFSM('end-game');
                            }
                        });

                    }
                    else {

                    }

                })();
                ////////////////////////start-game end
                break;

            case 'start-level':
                (function() {
                    if(fsm_step == 0) {
                        fsm_step++;

                        console.log('start level :' + (game_play.current_level + 1) );

                        ///////////////callback
                        if(option.startLevelCallback) {
                            option.startLevelCallback({level : game_play.current_level});
                        }
                        ///////////////////////

                        game_play.turn_count = 0;
                        game_play.current_level_def = game_play.level_def[game_play.current_level];

                        if(game_play.current_level_def.game_type == 'normal' &&
                            game_play.current_level_def.max_stand_count <= 0
                        ) { //종료 플래그 라운드

                            setFSM('game-over');

                        }
                        else {
                            //스태이지 시작음
                            if(game_play.current_level_def.sound.title.intro != 'none') {
                                theApp.method.sound.playFX( game_play.current_level_def.sound.title.intro);
                                console.log(game_play.current_level_def.sound.title.intro);
                            }
                            //배경음 스타트
                            if(game_play.current_level_def.sound.bgm != 'none') {
                                setTimeout(function() {
                                    gts_v1.ProcessMp3(JSON.stringify({action:'play',file_name:'/sdcard/GTS815/' + game_play.current_level_def.sound.bgm }));
                                },1000); //3초후에 연주시작
                            }
                            //타겟당 효과음 설정
                            var sndTable = [];
                            for(var i=0;i<game_play.current_level_def.unit.length;i++) {
                                sndTable[i] = "hit" + (game_play.current_level_def.unit[i].hit_snd+1);
                            }

                            hit_sound_effect_table = sndTable;

                            console.log('--timer start wait stand target--')
                            //완전히 다 쓰러질때까지 기다리기
                            wait_all_stand_timer_id = setTimeout(function() {
                                console.log('--timer finish wait stand target--')
                                setFSM('stand-target');
                                wait_all_stand_timer_id = -1;
                            },2000);
                        }
                    }
                    else {

                    }

                })();
                /////////////////////// start-level end
                break;
            //타겟 세우기 준비
            case 'stand-target':
                (function() {
                    if(fsm_step == 0) {

                        fsm_step++;

                        //콤보리셋
                        game_play.combo.combo_count = 0;
                        game_play.combo.hit_tick_start = 0;


                        //기존 타겟데이터 클리어 확인사살..
                        game_play.play_target = [];

                        //셔플정지
                        random_obj.no_shuffle();

                        var stand_target_number = Math.min(game_play.current_level_def.max_stand_count,random_obj.getTableSize());

                        for(var i=0;i<stand_target_number; i++) {
                            game_play.play_target.push(
                                {
                                    id: random_obj.get(),
                                    age: 0

                                }
                            );
                        }

                        //셔플재계
                        random_obj.yes_shuffle();

                        //타겟용 명령 코드만들기
                        var code ="";
                        var speed_table = [0x4d,0x4e,0x4f,0x50,0x52,0x53]; //무빙스피드 테이블

                        //타겟 상태 초기화..
                        for(var i=0;i<game_play.target_status.length;i++) {
                            game_play.target_status[i] = 0;
                        }

                        for(var i=0;i<game_play.play_target.length;i++) {

                            //일어선타겟표시
                            game_play.target_status[game_play.play_target[i].id] = 1;


                            var target_id = game_play.play_target[i].id + 0x30;
                            var unit_def = game_play.current_level_def.unit[game_play.play_target[i].id];

                            code +=  target_id +',85,';

                            //무빙
                            if(unit_def.type == 'moving' && unit_def.speed > 0) {


                                code +=  target_id +',' + speed_table[unit_def.speed-1] + ',';
                            }
                            else {
                            }
                        }

                        /*
                        //안일어난타겟은 넘겨짐 처리
                        for(var i=0;i<game_play.target_status.length;i++) {
                            if(!game_play.target_status[i]) {
                                code +=  target_id +',68,';
                            }
                        }
                        */


                        //맨마지막 ,표 제거
                        code = code.substring(0, code.length - 1);
                        async.waterfall([
                                function(next) {
                                    controller.directCommand({
                                        cmd : 'control',
                                        param : {
                                            code : code
                                        },
                                        callback : function(evt) {
                                            //console.log(evt);
                                            next(null);

                                        }
                                    });
                                }
                            ],
                            function() {
                                setFSM('play-game');
                            });

                    }
                    else {

                    }
                })();
                /////////////////////// stand-target end
                break;

            case 'play-game':
                (function() {

                    if(fsm_step == 0) {

                        fsm_step++;

                        //게임종료
                        theApp.event_listener.OnEndGame = function(evt) {

                            theApp.event_listener.OnEndGame = null;
                            context.reset();

                        }

                        //타겟히트
                        theApp.event_listener.OnHit = function(evt) {

                            //점수처리
                            //game_play.play_target[evt.target_index];
                            var combo_hit = {} //콤보힛 정보
                            var unit_def = game_play.current_level_def.unit[evt.target_index];
                            game_play.score += unit_def.score;

                            if(game_play.combo.hit_tick_start == 0) {
                                game_play.combo.hit_tick_start = (new Date()).getTime();
                            }
                            else {
                                var current_tick = (new Date()).getTime();

                                if((current_tick - game_play.combo.hit_tick_start) < game_play.combo.combo_tick_limit) {
                                    game_play.combo.combo_count++;

                                    combo_hit.delay = current_tick - game_play.combo.hit_tick_start;
                                    combo_hit.bonus = game_play.combo.combo_bonus[game_play.combo.combo_count-1];
                                    combo_hit.count = game_play.combo.combo_count;

                                    //보너스점수 추가
                                    game_play.score += combo_hit.bonus;

                                    //콤보효과음
                                    if(combo_hit.bonus > 0 ) {
                                        var combo_fx = game_play.combo.fx_snd[game_play.combo.combo_count-1];
                                        setTimeout(function() {
                                            console.log('play combo snd! :' + combo_fx);
                                            theApp.method.sound.playFX(combo_fx);
                                        },250)
                                        console.log( game_play.combo.combo_count + ' combo!');
                                    }
                                    ///////////////////////////////
                                }
                                else {
                                    game_play.combo.combo_count = 0;//콤보 취소
                                }

                                game_play.combo.hit_tick_start = current_tick;
                            }

                            //넘김효과음
                            theApp.method.sound.playFX(hit_sound_effect_table[evt.target_index]);

                            //무빙정지
                            if(unit_def.type == 'moving' && unit_def.speed > 0) {

                                var code =  (parseInt(evt.target_index)+48) + ',109';

                                controller.directCommand({
                                    cmd : 'control',
                                    param : {
                                        code : code
                                    },
                                    callback : function(evt) {
                                        //console.log(evt);
                                        //next(null);
                                    }
                                });
                            }

                            //점수화면갱신
                            if(option.scoreCallBack) {
                                option.scoreCallBack(
                                    {
                                        score : game_play.score,
                                        target_unit : unit_def,
                                        type : 'hit',
                                        combo_hit : combo_hit
                                    }
                                );
                            }

                            //넘어짐으로 다시 기록 처리
                            game_play.target_status[evt.target_index] = 0;

                            //다음턴 넘기는 여부 결정
                            var isClear = true;
                            for(var i=0;i<game_play.target_status.length;i++) {
                                if(game_play.target_status[i]) {
                                    isClear = false;
                                }
                            }

                            if(isClear) {
                                setFSM('next-turn');
                                theApp.event_listener.OnHit = null;
                            }

                            //////////////////////////////
                            // 다음 레벨 여부 검사
                            if(game_play.current_level_def.game_type == 'attack-score') {


                                if(game_play.score >= game_play.current_level_def.limit_score ) {

                                    //세워져 있는 타겟만 넘어뜨리기..
                                    var code ='';
                                    for(var i=0;i<game_play.target_status.length;i++) {

                                        if(game_play.target_status[i]) {

                                            code += (i + 0x30);
                                            code += ',68,';
                                        }
                                    }

                                    if(code != '') {

                                        //str = str.substring(0, str.length - 1);
                                        code = code.substring(0, code.length - 1);

                                        controller.directCommand({
                                            cmd : 'control',
                                            param : {
                                                code : code
                                            },
                                            callback : function(evt) {

                                                //다음 단계로..
                                                setFSM('next-level');

                                            }
                                        });
                                    }
                                    else {
                                        //다음 단계로..
                                        setFSM('next-level');
                                    }
                                }

                            }
                            /////////////////////////////////
                        }
                    }
                    else {

                        game_play.play_time += deltaTime;

                        if(game_play.current_level_def.game_type == 'attack-time') {

                            if(game_play.play_time > game_play.current_level_def.limit_time) {
                                //모든타겟정리
                                //clearAllTarget();
                                //fsm = 'next-level';
                                setFSM('next-level');
                            }
                        }

                        var isClear = true;
                        var code = '';

                        for(var i=0; i < game_play.play_target.length;i++) {

                            var target_obj =  game_play.play_target[i];
                            var unit_def = game_play.current_level_def.unit[target_obj.id];

                            //살아있는 타겟처리
                            if(target_obj.age >= 0 && game_play.target_status[target_obj.id] ) { //살아 있으면...

                                target_obj.age += deltaTime;

                                //나이가 들면..죽이기, 타임 아웃 처리
                                if(unit_def.life_time > 0) {
                                    if(target_obj.age > unit_def.life_time ) {

                                        //console.log('old die target...');

                                        target_obj.age = -1; //죽은상태로 전환...
                                        game_play.score -=  unit_def.miss; //감정 처리

                                        if(game_play.score < 0) {
                                            game_play.score = 0;
                                        }


                                        var target_id = 0x30 + target_obj.id;

                                        code += target_id + ',68,';

                                        //무빙이면 정지
                                        if(target_id == 0x30) {
                                            code += target_id + ',109,';
                                        }

                                        //점수화면갱신
                                        if(option.scoreCallBack) {
                                            option.scoreCallBack(
                                                {
                                                    score : game_play.score,
                                                    target_unit : unit_def,
                                                    type : 'miss'
                                                }
                                            );
                                        }
                                        //완전히 다내려간거 확인 한 다음 쓰러진 타겟으로 간주하기
                                        setTimeout((function() {

                                            var target_id = target_obj.id;

                                            return function() {
                                                game_play.target_status[target_id] = 0;
                                            }

                                        })(),1000);
                                    }
                                }
                                ///////////
                            }

                            //모두 쓰러 졌는지 확인하기
                            if(game_play.target_status[target_obj.id] != 0) {
                                isClear = false;
                            }

                        }/////////end for

                        //타겟 내리기
                        if(code != '') {

                            //마지막 문자제거
                            code = code.substring(0,code.length-1);

                            controller.directCommand({
                                cmd : 'control',
                                param : {
                                    code : code
                                },
                                callback : function(evt) {
                                    //console.log(evt);
                                }
                            });

                        }

                        //다음 턴으로
                        if(isClear && fsm != 'next-level') //이미 다음판으로 넘기기로 결정된게 아니였다면...
                        {
                            //이밴트무효
                            theApp.event_listener.OnHit = null;
                            setFSM('next-turn');
                        }
                    }
                })();
                ///////////////////////// play-game end
                break;
            case 'check-target':
                break;
            case 'next-turn':

                (function() {
                    if(fsm_step == 0) {

                        fsm_step++;

                        async.waterfall([
                                function(next) {
                                    next(null);
                                }
                            ],
                            function(error,result) {

                                game_play.turn_count++;
                                game_play.play_target = [];

                                if(game_play.current_level_def.max_turn_count != undefined &&
                                    game_play.current_level_def.game_type == 'normal') {

                                    if(game_play.turn_count >= game_play.current_level_def.max_turn_count) {
                                        setFSM('next-level');
                                    }
                                    else {
                                        setFSM('stand-target');
                                    }
                                }
                                else {
                                    setFSM('stand-target');
                                }
                            });
                    }
                    else {

                    }

                })();
                ///////////////////////// next-turn end

                break;
            case 'next-level':

                (function() {
                    if(fsm_step == 0) {
                        fsm_step++;



                        async.waterfall([
                                function(next) {


                                    console.log('go next level ! ');
                                    //배경음 끄기
                                    gts_v1.ProcessMp3(JSON.stringify({action:'stop'}));

                                    //레벨종료 멘트
                                    //theApp.method.sound.playFX("gameend");
                                    console.log('-- timer start --');
                                    setTimeout(function() {
                                        console.log('-- timer end --');
                                        next(null,{result : 'ok'});
                                    },500);

                                }
                            ],
                            function(error,result) {

                                game_play.current_level++;

                                //모든 레벨클리어
                                if(game_play.current_level >= game_play.level_def.length ) {

                                    fsm = 'end-game';

                                    //////////////////////////////
                                    if(option.gameClearCallBack) { option.gameClearCallBack( { game_play : game_play } );}
                                    ///////////////////////////
                                }
                                else {
                                    ///////////////////////
                                    if(option.levelClearCallBack) {option.levelClearCallBack({game_play : game_play});}
                                    ///////////////////////

                                    //다음판 시작전 모든 타겟 정리
                                    clearAllTarget(function(evt) {
                                        if(evt.result == 'ok') {

                                            console.log('-- timer start --');
                                            setTimeout(function() {
                                                console.log('-- timer end --');
                                                setFSM('start-level');
                                            },1500);
                                        }
                                        else {
                                            setFSM('end-game');
                                        }
                                    });
                                }
                            });
                        /////////////////////////end of async
                    }
                    else {

                    }
                })();
                ///////////////////////// next-level end
                break;

            case 'game-over':

                (function() {
                    if(fsm_step == 0) {

                        async.waterfall([
                            function(next) {

                                //배경음 끄기
                                gts_v1.ProcessMp3(JSON.stringify({action:'stop'}));
                                theApp.method.sound.playFX("gameover2");
                                next(null);
                            },
                            function(next) {

                                clearAllTarget(function(evt) {

                                    setTimeout(function() {
                                        theApp.Scene.score_panel.game_over_Action();
                                        if(option.gameStopCallBack) {
                                            option.gameStopCallBack(
                                                {
                                                    game_play : game_play
                                                }
                                            )
                                        }

                                        next(null);

                                    },2000);

                                });

                            }

                        ],function(err,result) {
                            setFSM('pre-ready')
                        });

                        fsm_step++;

                    }
                    else {

                    }

                })();
                //////////////////////// game-over end
                break;

            case 'end-game':

                (function() {
                    if(fsm_step == 0) {

                        fsm_step++;

                        async.waterfall(
                            [
                                function(next) {
                                    //배경음 끄기
                                    gts_v1.ProcessMp3(JSON.stringify({action:'stop'}));

                                    //theApp.method.sound.playFX("gameover2");
                                    theApp.method.sound.playFX("sns_gameend");

                                    next(null);

                                },
                                function(next) {
                                    //모든타겟정리
                                    clearAllTarget(function(evt) {
                                        if(evt.result == 'ok') {

                                            setFSM('wait-all-target-down-before-pre-ready');
                                            if(option.gameStopCallBack) {
                                                option.gameStopCallBack(
                                                    {
                                                        game_play : game_play
                                                    }
                                                )
                                            }

                                        }
                                        else {
                                            setFSM('end-game');
                                        }
                                        next(null);

                                    });
                                }
                            ],
                            function(error,result) {

                            }
                        );

                    }
                    else {

                    }

                })();
                ////////////////////////// end-game end
                break;
            case 'wait-all-target-down-before-pre-ready':

                (function() {
                    //타겟이 완전히 다 정리될때 까지 기다려야...
                    if(fsm_step == 0) {
                        fsm_step++;
                        wait_alldown_timer_id = setTimeout(function() {
                            console.log('-- all down timer complete --');
                            setFSM('pre-ready');
                            gts_v1.ProcessMp3(JSON.stringify({action:'stop'}));

                        },5000);
                        console.log('-- wait all down timer start -- :' + wait_alldown_timer_id);
                    }
                    else {

                    }

                })();
                ////////////////////////////// wait-all-target-down-before-pre-ready end
                break;
            default:
                break;
        }

        prevTime = currentTime;

        requestAnimationFrame(loop);

    }



    console.log('success setup');
    loop();

}