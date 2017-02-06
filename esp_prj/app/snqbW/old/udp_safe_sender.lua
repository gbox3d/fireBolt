AsyncSender_version = "102"
--버퍼에 있는대로 모아서 보내기
function AsyncSender_Safe_udp(option)
    local test_set = false;
    local tempBuf = "";
    local _timer = tmr.create()
    return function(data,_port,_ip)
        local socket = option.getsocket()
        function _send(data)
            --print("buf : " .. data)
            if test_set == true then tempBuf = tempBuf..data --print(tempBuf)
            else
                local _data = tempBuf..data;tempBuf=""
                if _data:len()>0 then
                    test_set=true
                    --print("send :  " .._data)
                    socket:send(_port,_ip,_data,
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