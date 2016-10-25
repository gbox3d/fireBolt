dofile('tcp_safe_sender.lua')


wifi.setphymode(wifi.PHYMODE_B)
wifi.sleeptype(wifi.NONE_SLEEP)

wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE")  end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL")  end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function()
    print("STATION_GOT_IP")

    print(wifi.sta.getip())

end)
wifi.sta.eventMonStart()

wifi.setmode(wifi.STATION)
wifi.sta.config("RD2GUNPOWER","808671004")
wifi.sta.connect()

function testApp()

    local udp_conn =net.createConnection(net.UDP)
    udp_conn:on("receive",function(cu,c) print('bind :' .. c) end)

    -- 첫번째 인자는 원격지의 포트번호이다.
    udp_conn:connect(1471,"192.168.9.2")

    local test_str = "123456789abcd";

    test_str:gsub(".", function(code)
        udp_conn:send(code);
        end)
end

function tcpTest()

    local tcp_conn = net.createConnection(net.TCP)
    tcp_conn:on("receive", function(sck, c) print(c) end)
    tcp_conn:on("connection", function(sck, c)
        -- Wait for connection before sending.
        --sck:send("GET / HTTP/1.1\r\nHost: 192.168.0.66\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")

        local test_str = "123456789abcdefg";
        local safe_sender_1 = AsyncTCPSender_Safe_cs( {getsocket = function() return tcp_conn end,delay=50} )
        test_str:gsub(".", function(code)
            safe_sender_1(code)
        end)

        tmr.alarm(0,5000,tmr.ALARM_SINGLE, function() sck:close() end);


    end)

    tcp_conn:connect(1472,"192.168.9.2")

end


