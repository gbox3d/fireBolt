dofile("config.lua")
dofile('tcp_safe_sender.lua')
dofile('app.lua')
dofile('BeanconHttpServer.lua')
dofile('blinker.lua')

status_led = 0;
gpio.mode(status_led,gpio.OUTPUT)

gpio.mode(1,gpio.OUTPUT)
gpio.write(1,0)

device_id = "udbCon"..node.chipid();
wifi.setmode(wifi.STATIONAP)
wifi.ap.config({ssid=device_id,pwd=nil})

--wifi.setphymode(wifi.PHYMODE_B)
wifi.setphymode(wifi.PHYMODE_G)
wifi.sleeptype(wifi.NONE_SLEEP)

--wifi.sta.autoconnect(1)
wifi.sta.disconnect()

wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE")
    blinker_status_led("start")
end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL")  end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function()
    print("STATION_GOT_IP")
    blinker_status_led("stop-on")
    local ip,_,_ = wifi.sta.getip()
    local _,_, ip_class = string.find(ip,"(%d+%.%d+%.%d+.)");
    print(ip_class)
    start_UdpController(device_id,ip,ip_class,app_config.port,app_config.safe_delay)
end)

if(app_config.ssid ~= nil) then
    wifi.sta.eventMonStart()
    wifi.sta.config(app_config.ssid,app_config.passwd,1)
    wifi.sta.connect()
end

http_server = BeaconHttpServer({
    callback = function(json_obj)end
})
http_server.startup();

print('start udbeeCon version : 0.0.2')
--for word in c:gmatch("%d.d.d.") do print(word) end
-- = c:find("(%d).(%d+).(%d+).");
-- = c:find("(%d+%.%d+%.%d+.)");


blinker_status_led=blink_led({pin=status_led,delay=500,tmid=1,max_count=0})
blinker_status_led("start")
