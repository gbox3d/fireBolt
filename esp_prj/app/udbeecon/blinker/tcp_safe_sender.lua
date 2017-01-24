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
--한바이트씩보내기
function AsyncTCPSender_Safe_cs(option)
    local test_set = false;
    local tempBuf = "";
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
                            test_set=false;
                            if tempBuf:len()>0 then
                                --print("remain :  " ..tempBuf)
                                tmr.alarm(2,option.delay,0,function() _send("") end)
                            end
                        end
                    )
                end
            end
        end
        if socket~=nil then _send(data) end
    end --function
end




