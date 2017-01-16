wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE") end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL") end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function() if OnSTAGetIp then OnSTAGetIp() end  end)

wifi.sta.eventMonStart()

function OnSTAGetIp()
    print("STATION_GOT_IP:" .. wifi.sta.getip())
end

wifi.setmode(wifi.STATION)

cfg = {ip = "192.168.9.20",netmask = "255.255.255.0",gateway="192.168.9.1"}
wifi.sta.setip(cfg)
wifi.sta.config("--","--",1)
wifi.sta.connect()

wifi.setphymode(wifi.PHYMODE_B) -- 모드 N에 비해 한바이트씩 전송하는것은 오히려 속도가 빠름
wifi.sleeptype(wifi.NONE_SLEEP) -- 슬립모드방지,이 옵션을 설정하지않으면 주기적으로 느려짐


function startApp()

    cu=net.createConnection(net.UDP)
    cu:on("receive",function(cu,c) print('bind 1 :' .. c) end)
    -- 첫번째 인자는 원격지의 포트번호이다.
    cu:connect(1471,"192.168.9.3")

    cu2=net.createConnection(net.UDP)
    cu2:on("receive",function(cu,c) print('bind 2 :' .. c) end)
    -- 첫번째 인자는 원격지의 포트번호이다.
    cu2:connect(1471,"192.168.9.3")

    count = 0
    tmr.alarm(0,1000,1,function()
        cu:send("hello")
        cu2:send("hello")
        print('send ok..and ' .. node.heap() )
        count = count+1
        if count > 20 then
            tmr.stop(0)
        end
    end)


end