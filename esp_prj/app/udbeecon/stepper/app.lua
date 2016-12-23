
-------
--버퍼에 있는대로 모아서 보내기
function AsyncTCPSender_Safe(option)
    local test_set = false;
    local tempBuf = "";
    return function(data)
        local socket = option.getsocket()
        function _send(data)
            if test_set == true then tempBuf = tempBuf..data --print(tempBuf)
            else
                local _data = tempBuf..data;tempBuf=""
                if _data:len()>0 then
                    test_set=true
                    socket:send(_data,
                        function()
                            test_set=false;
                            if tempBuf:len()>0 then
                                --print("remain :  " ..tempBuf)
                                _send("") end
                        end
                    )
                end
            end
        end
        if socket~=nil then _send(data) end
    end --function
end
------------------------------

function start_stepper()
    local stepper_pins = {0,5,6,7}
    -- half or full stepping
    local step_states4 = {
        {1,0,0,1},
        {1,1,0,0},
        {0,1,1,0},
        {0,0,1,1}
    }
    local step_states8 = {
        {1,0,0,0},
        {1,1,0,0},
        {0,1,0,0},
        {0,1,1,0},
        {0,0,1,0},
        {0,0,1,1},
        {0,0,0,1},
        {1,0,0,1},
    }

    local step_states = step_states8 -- choose stepping mode
    local step_numstates = 8 -- change to match number of rows in step_states

    --step_states = step_states
    --step_numstates = 4
    local step_delay = 20 -- choose speed
    local step_state = 0 -- updated by step_take-function
    local step_direction = 1 -- choose step direction -1, 1
    local step_stepsleft = 0 -- number of steps to move, will de decremented
    local step_timerid = 4 -- which timer to use for the steps
    local OnEndcallback = nil

    -- setup pins
    for i = 1, 4, 1 do
        gpio.mode(stepper_pins[i],gpio.OUTPUT)
    end

    -- turn off all pins to let motor rest
    local function step_stopstate()
        for i = 1, 4, 1 do
            gpio.write(stepper_pins[i], 0)
        end
    end

    -- make stepper take one step
    local function step_take()

        -- jump to the next state in the direction, wrap
        step_state = step_state + step_direction
        if step_state > step_numstates then
            step_state = 1;
        elseif step_state < 1 then
            step_state = step_numstates
        end

        --print("step " .. step_state)
        -- write the current state to the pins
        for i = 1, 4, 1 do
            gpio.write(stepper_pins[i], step_states[step_state][i])
        end

        -- might take another step after step_delay
        step_stepsleft = step_stepsleft-1
        if step_stepsleft > 0 then
            tmr.alarm(step_timerid, step_delay, 0, step_take )
        else
            step_stopstate()
            if(OnEndcallback) then OnEndcallback() end

        end
    end

    -- public method to start moving number of 'int steps' in 'int direction'
    function stepper_move(steps, direction, delay,endCallback)
        tmr.stop(step_timerid)
        step_stepsleft = steps
        step_direction = direction
        step_delay = delay
        step_take()

        if(endCallback) then OnEndcallback = endCallback end;

    end

    -- public method to cancel moving
    function stepper_stop()
        tmr.stop(step_timerid)
        step_stepsleft = 0
        step_stopstate()
    end
end
------------------------------
local safe_sender;
local safe_sender_broker;
local udp_server;
local blinker;
local isCheckin = false;
local send_pkt = {
    pkt='none'
};

function start_UdpController(device_id,ip,port,safe_delay)

    start_stepper();

    print(port)
    local _,_, ip_class = string.find(ip,"(%d+%.%d+%.%d+.)");
    print(ip_class)
    if udp_server == nil then
        udp_server = net.createServer(net.UDP)
        udp_server:on("receive",
            function(socket,c)
                local jsn_obj = cjson.decode(c)
                if jsn_obj.fn == 'test' then
                    stepper_move(1024,(1),1,function() print('end of step moter move') end)
                elseif jsn_obj.fn =='stm' then
                    send_pkt.pkt='stm'
                    stepper_move(jsn_obj.step,jsn_obj.dir,jsn_obj.delay,function()
                        --send_pkt = {pkt="stm-ok"}
                        --safe_sender_broker(cjson.encode(send_pkt))
                        --print('end of step moter move')
                        send_pkt.pkt='none'
                    end)
                elseif jsn_obj.fn =='stp' then
                    stepper_stop()
                elseif jsn_obj.fn == 'checkin' then
                    if(isCheckin == false) then
                        isCheckin = true;
                        local udp_socket_broker=net.createConnection(net.UDP)
                        udp_socket_broker:connect(port,jsn_obj.ip)
                        safe_sender_broker = AsyncTCPSender_Safe({getsocket = function() return udp_socket_broker end,delay=safe_delay})
                        local function _loop()
                            safe_sender_broker(cjson.encode(send_pkt))
                            if(isCheckin == true) then tmr.alarm(1,150,tmr.ALARM_SINGLE,_loop) end
                        end
                        _loop()
                    end

                elseif jsn_obj.fn=='gpio-set' then
                    gpio.mode(jsn_obj.pin,gpio.OUTPUT)
                    gpio.write(jsn_obj.pin,jsn_obj.val)
                end


            end)
        udp_server:listen(port)
    end



    --broadcast
    local udp_socket=net.createConnection(net.UDP)
    udp_socket:connect(port,ip_class .. "255")
    safe_sender = AsyncTCPSender_Safe({getsocket = function() return udp_socket end,delay=safe_delay})

    local send_pkt = {
        pkt = "bcast",
        ip = ip,
        device_id = device_id
    }
    --local str_info = cjson.encode(send_pkt)
    tmr.alarm(0,3000,tmr.ALARM_AUTO,function () safe_sender(cjson.encode(send_pkt)) end)
    
end
