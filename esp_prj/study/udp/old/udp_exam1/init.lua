wifi.setmode(wifi.STATION)
wifi.sta.config("--","--")
wifi.sta.connect()

--s 는 server sub module 이다.
--서버형식으로 자신의 포트를 지정할수 있다.
--이후에 cu를 통해 send를 해도 여기서 정해준 포트로 데이터가 들어오게 된다.
--단 cu에서 지정한 receive 콜백은 무시된다. s로 지정한 콜백만 유효하다.

--s=net.createServer(net.UDP)
--s:on("receive",function(s,c) print('listen : ' ..c) end)
--s:listen(5683)

-- cu 는 socket sub module 이다.
-- 포트를 바인딩할수없고 자동으로 바인딩포트가 할당된다.
-- 상대방에 접속해서 자신의 포트를 알려줄때까지는 포트 정보를 알수가없다.
-- bind 함수가 따로 존재하지 않는다.
cu=net.createConnection(net.UDP)
cu:on("receive",function(cu,c) print('bind :' .. c) end)

-- 첫번째 인자는 원격지의 포트번호이다.
cu:connect(1471,"192.168.9.3")
count = 0
tmr.alarm(0,1000,1,function()
    cu:send("hello")
    print('send ok..and ' .. node.heap() )
    count = count+1
    if count > 20 then
        tmr.stop(0)
    end
end)