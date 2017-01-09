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

--받기 설정
local udp_socket = net.createUDPSocket()
udp_socket:on("receive",function(cu,c) print('recv : ' .. c) end)
udp_socket:listen(1471) -- 첫번째 인자는 데이터를 받을 로컬포트 번호이다

function test()
    udp_socket:send(1471,test_ip,test_str)
end
