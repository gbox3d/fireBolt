dofile("config.lua")
dofile('BeanconHttpServer.lua')
dofile('tcp_safe_sender.lua')
dofile('app.lua')

wifi.setmode(wifi.STATIONAP)
wifi.ap.config({ssid="gboxesp"..node.chipid(),pwd=nil})

wifi.sta.autoconnect(0)
wifi.sta.disconnect()

if(app_config.ap_cfg ~= nil) then

     local cfg =  {
            ip="192.168.".. app_config.ap_cfg[1] ..".1",
            netmask="255.255.255.0",
            gateway="192.168." .. app_config.ap_cfg[1] .. ".1"
        }
    wifi.ap.setip(cfg)
end

if(app_config.station_cfg ~=nil ) then
    wifi.sta.config(app_config.station_cfg[1],app_config.station_cfg[2])
    if(app_config.station_cfg[3] ~= nil) then
        local cfg = {

            ip="192.168.".. app_config.station_cfg[3] .."." .. app_config.station_cfg[4],
            netmask="255.255.255.0",
            gateway="192.168." .. app_config.station_cfg[3] .. ".1"

        }
        wifi.sta.setip(cfg)
    end
    wifi.sta.connect()

end

wifi.setphymode(wifi.PHYMODE_B)
wifi.sleeptype(wifi.NONE_SLEEP)

wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE")  end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL")  end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function() print("STATION_GOT_IP") end)
wifi.sta.eventMonStart()



http_server = BeaconHttpServer({
    callback = function(json_obj)
        --http://192.168.9.14/json?data={"fn":"getip"}
        if(json_obj.fn == 'getip') then local temp = {result='ok',ip=wifi.sta.getip() } local str_res = cjson.encode(temp) return str_res
        elseif(json_obj.fn=='start') then start_Udp2Uart(app_config.remote.ip,app_config.remote.port)
        elseif(json_obj.fn=='stop') then uart.setup(0,app_config.baud,8,0,1,1) uart.on("data")
        end

    end
})
http_server.startup();

print('start udbee version : 0.0.2')
if(app_config.autostart == true) then start_Udp2Uart(app_config.remote.ip,app_config.remote.port,app_config.parser,app_config.safe_delay) end


