dofile("config.lua")

wifi.setmode(wifi.STATIONAP)
wifi.ap.config({ssid="gunesp"..node.chipid(),pwd=nil})

wifi.sta.autoconnect(0)
wifi.sta.disconnect()

wifi.sta.config(station_mode_info.conn.ssid,station_mode_info.conn.passwd)
if(station_mode_info.cfg) then
    wifi.sta.setip(station_mode_info.cfg)
end
wifi.sta.connect()

wifi.setphymode(wifi.PHYMODE_B) -- 모드 N에 비해 한바이트씩 전송하는것은 오히려 속도가 빠름
wifi.sleeptype(wifi.NONE_SLEEP) -- 슬립모드방지,이 옵션을 설정하지않으면 주기적으로 느려짐

wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE")  end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL")  end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function() print("STATION_GOT_IP") end)
wifi.sta.eventMonStart()

dofile('BeanconHttpServer.lua')

http_server = BeaconHttpServer({
    callback = function(json_obj)
        if(json_obj.fn == 'getip') then
            local temp = {result='ok',ip=wifi.sta.getip() }
            local str_res = cjson.encode(temp)
            return str_res
        end

    end
})
http_server.startup();