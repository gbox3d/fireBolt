--
-- Created by IntelliJ IDEA.
-- User: gunpower
-- Date: 2016. 10. 25.
-- Time: PM 2:58
-- To change this template use File | Settings | File Templates.
--
dofile('app.lua')

wifi.setphymode(wifi.PHYMODE_B)
wifi.sleeptype(wifi.NONE_SLEEP)

wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE")  end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL")  end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function()
    print("STATION_GOT_IP")
    print(wifi.sta.getip())
end)
wifi.sta.eventMonStart()

wifi.setmode(wifi.STATION)
wifi.sta.config("RD2GUNPOWER","808671004")
wifi.sta.connect()

