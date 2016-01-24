wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE") end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL") end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function() if OnSTAGetIp then OnSTAGetIp() end  end)

wifi.sta.eventMonStart()

wifi.setmode(wifi.STATION)
wifi.sta.config("9-2001","1403028064")
wifi.sta.connect()

function OnSTAGetIp()
    print("STATION_GOT_IP:" .. wifi.sta.getip())

    udp_socket=net.createConnection(net.UDP)
    udp_socket:connect(5683,"192.168.219.255")

    tmr.alarm(0,2000,1,function()
        udp_socket:send("hello")
        print('send ok..and ' .. node.heap() )
    end)

end

--[[
udp_socket=net.createConnection(net.UDP)
udp_socket:connect(5683,"192.168.219.255")

tmr.alarm(0,2000,1,function()
    udp_socket:send("hello")
    print('send ok..and ' .. node.heap() )
end)
--]]


