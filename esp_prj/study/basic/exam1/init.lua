wifi.sleeptype(wifi.NONE_SLEEP)
wifi.setmode(wifi.STATION)wifi.sta.config("esptest","123456789")wifi.sta.setip({ip="192.168.9.5",netmask="255.255.255.0",gateway=""})wifi.sta.connect()
print("connect :" .. wifi.sta.getip())

cu=net.createConnection(net.UDP)
cu:on("receive",function(cu,c) print('receive packet :' .. c) end)
-- 첫번째 인자는 원격지의 포트번호이다.
cu:connect(1471,"192.168.9.1")
tmr.alarm(0,1000,1,function() msg="hello"..wifi.sta.getip() cu:send(msg) print(msg) end)


