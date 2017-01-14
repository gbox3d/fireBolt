dofile('async_safe_sender.lua')


wifi.setphymode(wifi.PHYMODE_B)
wifi.sleeptype(wifi.NONE_SLEEP)
wifi.setmode(wifi.STATION)

wifi.sta.autoconnect(1)
wifi.sta.disconnect()
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
wifi.sta.config("myssid","mypasswd")
wifi.sta.connect()

test_str = "123456789abcd";
test_ip = "192.168.9.3"



local udp_listener = net.createServer(net.UDP)
udp_listener:on("receive",function(cu,c) print('recv : ' .. c) end)
udp_listener:listen(1471)


function udpTest()

    local udp_conn =net.createConnection(net.UDP)
    -- 첫번째 인자는 원격지의 포트번호이다.
    udp_conn:connect(1471,test_ip)

    local safe_sender_1 = AsyncSender_Safe( {getsocket = function() return udp_conn end} )
    test_str:gsub(".", function(code)
        --udp_conn:send(code);
        safe_sender_1(code)
        end)

    --tmr.create():alarm(2000,tmr.ALARM_SINGLE,udpTest2)


end

function udpTest2()
    local udp_conn =net.createConnection(net.UDP)
    -- 첫번째 인자는 원격지의 포트번호이다.
    udp_conn:connect(1471,test_ip)
    local safe_sender_2 = AsyncSender_Safe_cs( {getsocket = function() return udp_conn end,delay=50} )
    test_str:gsub(".", function(code)
        --udp_conn:send(code);
        safe_sender_2(code)
    end)
end


function tcpTest()

    local tcp_conn = net.createConnection(net.TCP)
    tcp_conn:on("receive", function(sck, c) print('recv : ' .. c) end)
    tcp_conn:on("connection", function(sck, c)
        -- Wait for connection before sending.
        --sck:send("GET / HTTP/1.1\r\nHost: 192.168.0.66\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")

        --local test_str = "123456789abcdefg";
        local safe_sender_1 = AsyncSender_Safe( {getsocket = function() return tcp_conn end} )
        test_str:gsub(".", function(code)
            safe_sender_1(code)
        end)

        tmr.create():alarm(2000,tmr.ALARM_SINGLE, function()
            local safe_sender_2 = AsyncSender_Safe_cs( {getsocket = function() return tcp_conn end,delay=50} )
            test_str:gsub(".", function(code)
                safe_sender_2(code)
            end)

        end);


        tmr.create():alarm(5000,tmr.ALARM_SINGLE, function() sck:close() end);


    end)

    tcp_conn:connect(1472,test_ip)

end


