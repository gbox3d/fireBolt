wifi.setmode(wifi.SOFTAP)
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
wifi.ap.config({ssid="esptest",pwd="123456789"})
wifi.ap.setip({ip="192.168.9.1",netmask="255.255.255.0",gateway="192.168.9.1"})
print("ap mode test")
tmr.alarm(0, 1000, 0, function() print("hello world") end )

s=net.createServer(net.UDP)
s:on("receive",function(s,c) print('receive packet : ' ..c) end)
s:listen(1471)
