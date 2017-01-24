dofile('udp_safe_sender.lua')


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
wifi.sta.config("RD2GUNPOWER","808671004")
wifi.sta.connect()

test_str = "123456789abcd";
test_ip = "192.168.9.3"


local udp_socket = net.createUDPSocket()
udp_socket:on("receive",function(cu,c) print('recv : ' .. c) end)
--udp_socket:on("received",function(data,port,ip) print('recv : ' .. c .. ',' .. port .. ',' .. ip) end)
udp_socket:listen(1471)
local safe_sender_1 = AsyncSender_Safe_udp( {getsocket = function() return udp_socket end} )

function udpTest()

    test_str:gsub(".", function(code)
        --udp_conn:send(code);
        safe_sender_1(1471,test_ip,code)
    end)

    --tmr.create():alarm(2000,tmr.ALARM_SINGLE,udpTest2)
end


