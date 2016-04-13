dofile("config.lua")
dofile('tcp_safe_sender.lua')
dofile('app.lua')
dofile('BeanconHttpServer.lua')

gpio.mode(0,gpio.OUTPUT,gpio.PULLUP)
gpio.write(0,gpio.LOW)


device_id = "udbCon"..node.chipid();
wifi.setmode(wifi.STATIONAP)
wifi.ap.config({ssid=device_id,pwd=nil})

wifi.sta.autoconnect(1)
wifi.sta.disconnect()
wifi.sta.connect()

wifi.setphymode(wifi.PHYMODE_B)
wifi.sleeptype(wifi.NONE_SLEEP)

wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE")  end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL")  end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function()
    print("STATION_GOT_IP")
    local ip,_,_ = wifi.sta.getip()
    local _,_, ip_class = string.find(ip,"(%d+%.%d+%.%d+.)");
    print(ip_class)
    start_UdpController(device_id,ip,ip_class,app_config.port,app_config.safe_delay)
end)
wifi.sta.eventMonStart()


http_server = BeaconHttpServer({
    callback = function(json_obj)end
})
http_server.startup();

print('start udbeeCon version : 0.0.1')


--for word in c:gmatch("%d.d.d.") do print(word) end
-- = c:find("(%d).(%d+).(%d+).");
-- = c:find("(%d+%.%d+%.%d+.)");
