wifi.setmode(wifi.STATION)

wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE") end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL") end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function() if OnSTAGetIp then OnSTAGetIp() end  end)
wifi.sta.eventMonStart()


wifi.sta.config("--","--")
wifi.sta.connect()

function OnSTAGetIp()
    print("STATION_GOT_IP:" .. wifi.sta.getip())
    --[[
    udp_socket=net.createConnection(net.UDP)
    udp_socket:connect(5683,"192.168.42.14")


    tmr.alarm(0,10,1,function()
        udp_socket:send("hellowrold0123456789)!@#$%^&*(AB")
        --print('send ok..and ' .. node.heap() )
    end)
    --]]


end

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

safe_sender = AsyncTCPSender_Safe({getsocket = function() return work_socket end})

function startApp()
    work_socket = net.createConnection(net.UDP);work_socket:connect(5683,"192.168.42.14")
    --local work_socket = nil;
    --print("udp  app started version 0.0.2  :" ..jsn_obj.ip..':'..jsn_obj.port)
    if udp_server == nil then
        udp_server = net.createServer(net.UDP)
        udp_server:on("receive",function(socket,c) uart.write(0,c) end)
        udp_server:listen(1471)
    end
    --uart.on("data", string.char(0x0d),
    uart.setup(0,115200,8,0,1,0)
    uart.on("data","}",
        function(data)
            safe_sender(data)
            --[[
            if data ~= "{\"I\":1,\"S\":\"D\",\"T\":0}" then
                safe_sender("{\"I\":1,\"S\":\"E\",\"T\":0}")
            else
                safe_sender("{\"I\":1,\"S\":\"U\",\"T\":0}")
            end
            --]]
            --safe_sender("{\"I\":1,\"S\":\"U\",\"T\":0}")
        end,0)
    --uart.on("data", 0, function(data) work_socket:send(data) end,0)
    --tmr.alarm(0,0,1000,0,function() uart.write(0,"{\"C\":\"R\"}")  end) --데이터전송시작
end