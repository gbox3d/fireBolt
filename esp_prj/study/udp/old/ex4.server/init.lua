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

    error_count = 0;
    count = 0;
    udp_server = net.createServer(net.UDP)
    udp_server:on("receive",function(socket,c)
        --print(c)
        if c ~= [[{"C":"S","I":255,"V":126}]] then error_count = error_count+1 print("error :" .. error_count)
        else
            --print(count)
        end

        count = count+1
    end)
    udp_server:listen(1471)

end