wifi.setmode(wifi.STATIONAP)
--wifi.setmode(wifi.SOFTAP)

-- ap setup
cfg={}
cfg.ssid="esptest"
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
cfg.pwd="12345678"
wifi.ap.config(cfg)
--ap 모드에서 기본 주소는 192.168.4.1 이 됨



s=net.createServer(net.UDP)
s:on("receive",function(s,c) print('listen : ' ..c) end)
s:listen(5683)

-- station setup
--[[
cfg_station = {
    ip = "192.168.9.108",
    netmask = "255.255.255.0",
    gateway="192.168.9.177"
}
--]]
--wifi.sta.setip(cfg_station)
wifi.sta.connect()
wifi.sta.config("GUNPOWER_PI_1","")

tmr.alarm(0,1000,1 ,function() ip = wifi.sta.getip() if ip ~= nil then tmr.stop(0) print('connect ' .. ip) else print('connecting...') end  end)