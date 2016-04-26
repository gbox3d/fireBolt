
wifi.setmode(wifi.STATIONAP)

-- nil 을 주면 패스워드 설정안해도됨
--192.168.4.1
wifi.ap.config({ssid="esptest",pwd=nil})
--wifi.ap.setip({ip="192.168.9.1",netmask="255.255.255.0",gateway="192.168.9.1"})
print("station ap mode test")

cu=net.createConnection(net.UDP)
--cu:on("receive",function(cu,c) print('bind :' .. c) end)
-- 첫번째 인자는 원격지의 포트번호이다.
cu:connect(5735,"192.168.9.2")


s=net.createServer(net.UDP)
s:on("receive",function(s,c)
    print('receive packet : ' ..c)
    cu:send(c)
end)
s:listen(5630)

wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE") end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL") end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function() print("STATION_GOT_IP")end)
wifi.sta.eventMonStart()

cfg = {ip = "192.168.9.60",netmask = "255.255.255.0",gateway="192.168.9.1"}
wifi.sta.setip(cfg)
wifi.sta.config("RD2GUNPOWER","808671004",1)
wifi.sta.connect()

--wifi.setphymode(wifi.PHYMODE_B) -- 모드 N에 비해 한바이트씩 전송하는것은 오히려 속도가 빠름
--wifi.sleeptype(wifi.NONE_SLEEP) -- 슬립모드방지,이 옵션을 설정하지않으면 주기적으로 느려짐