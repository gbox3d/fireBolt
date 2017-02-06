AsyncSender_version = "102"
--버퍼에 있는대로 모아서 보내기
function AsyncSender_Safe(option)
    local test_set = false;
    local tempBuf = "";
    local _timer = tmr.create()
    return function(data)
        local socket = option.getsocket()
        function _send(data)
        --print("buf : " .. data)
            if test_set == true then tempBuf = tempBuf..data --print(tempBuf)
            else
                local _data = tempBuf..data;tempBuf=""
                if _data:len()>0 then                
                    test_set=true
                    --print("send :  " .._data)
                    socket:send(_data,
                        function()
                        --need cool time ?
                            _timer:alarm(1,tmr.ALARM_SINGLE,function() 
                                test_set=false 
                                if tempBuf:len()>0 then
                                    --print("remain :  " ..tempBuf)
                                    _send("") end                            
                                end)
                            --print("send ok :  " .._data)
                        end
                    )
                    --_timer:alarm(5,tmr.ALARM_SINGLE,function() 
                    --end)
                    
                end
            end
        end
        if socket~=nil then _send(data) end
    end --function
end

--한바이트씩보내기
function AsyncSender_Safe_cs(option)
    local test_set = false;
    local tempBuf = "";
    local _timer = tmr.create()
    return function(data)
        local socket = option.getsocket()
        function _send(data)
            if test_set == true then tempBuf = tempBuf..data --print(tempBuf)
            else
                tempBuf = tempBuf..data;
                if tempBuf:len()>0 then
                    test_set=true
                    local pkt =tempBuf:sub(1,1);
                    if 1 < tempBuf:len() then tempBuf = tempBuf:sub(2,tempBuf:len())
                    else tempBuf="" end
                    socket:send(pkt,
                        function()
                            _timer:alarm(option.delay,tmr.ALARM_SINGLE,function()
                                test_set=false;
                                if tempBuf:len()>0 then
                                    --_send("") 
                                    --print("remain :  " ..tempBuf)
                                
                                    _send("")
                                    --tmr.create():alarm(option.delay,tmr.ALARM_SINGLE,function() _send("") end)
                                end
                            end)
                        end)
                end
            end
        end
        if socket~=nil then _send(data) end
    end --function
end




