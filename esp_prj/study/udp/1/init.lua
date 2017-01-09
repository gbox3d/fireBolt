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
wifi.sta.config("-","-")
wifi.sta.connect()

test_str = "123456789abcd";
test_ip = "192.168.9.3"

--[[
-- 현재 nodemcu에서 udp는 tcp소켓과 API 인터패이스가 혼용되어 구현되어있다
 아래와 같이 다분히 tcp스럽게 써야한다. 리슨하는 포트에 대한 소켓을따로 만들어야만 원하는 포트로 리슨을 할수있다.
 createConnection 으로 생성한 udp 소켓( sub socket)은 리슨(bind) 함수가 아직제공되지않아서 서버로 따로 만들어야한다.(단지 receive 이벤트만 지정가능하다)
 또한 createServer 로 생성한 서버소켓은 send함수가 없어서 send용 소켓(sub socket은 send가 있다.)을 따로 만들어야한다.(다행이 udp는 다수소켓생성이 가능하다.)

 <위의 내용은 2017.1 월상황임 현재 dev브랜치에서 새로운 udp소켓을 작업중이므로 곧 개선 될것이라 예상됨.>

]]--

--받기 설정
local udp_listener = net.createServer(net.UDP)
udp_listener:on("receive",function(cu,c) print('recv : ' .. c) end)
udp_listener:listen(1471) -- 첫번째 인자는 데이터를 받을 로컬포트 번호이다

--보내기 설정
local udp_sender =net.createConnection(net.UDP)
udp_sender:connect(1471,test_ip) -- 첫번째 인자는 원격지의 포트번호이다.

function test()
    udp_sender:send(test_str)
end


