wifi.setmode(wifi.STATION)
wifi.sta.autoconnect(0)
wifi.sta.disconnect()

--cfg = {ip = "192.168.9.20",netmask = "255.255.255.0",gateway="192.168.9.1"}
--wifi.sta.setip(cfg)
function conn_helper_1()
    local current_ap_index = 1;
    aplist = {
        {name="ollehWiFi",ssid=""},
        {name="T wifi zone",ssid=""},        
        {name="9-2001",ssid=""},        
    }

    local step_connecting = 0;
    function connectNextAp()
        local apinfo = aplist[current_ap_index]
        wifi.sta.config(apinfo.name,apinfo.ssid,0)
        wifi.sta.connect();step_connecting=0
        print('try..'..apinfo.name)
        current_ap_index = current_ap_index+1
        if(current_ap_index > table.getn(aplist)) then
            current_ap_index = 1;
        end
    end


    function startConnectHelper()


        function connect_helper()
            print('do connecting helper...')
            --tmr.stop(6)
            --wifi.sta.disconnect()

            local status= wifi.sta.status();
            if (status == wifi.STA_CONNECTING) then
                if(step_connecting == 0) then
                    step_connecting = 1;
                else
                    wifi.sta.disconnect()
                    step_connecting = 0;
                end

            end
            tmr.alarm(6,5000,0,connect_helper)
        end

        tmr.stop(6)
        wifi.sta.disconnect()
        connectNextAp()
        connect_helper()
    end

    wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE") connectNextAp() end)
    wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
    wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
    wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
    wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL") end)
    wifi.sta.eventMonReg(wifi.STA_GOTIP, function() print("STATION_GOT_IP") end)
    wifi.sta.eventMonStart()
end


function conn_helper_2()

    local current_ap_index = 1;
    local aplist = {}
    local step_connecting = 0;

    wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE") connectNextAp() end)
    wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") end)
    wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") connectNextAp() end)
    wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") connectNextAp() end)
    wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL") connectNextAp() end)
    wifi.sta.eventMonReg(wifi.STA_GOTIP, function() print("STATION_GOT_IP") end)
    wifi.sta.eventMonStart()

    function connectNextAp()
        local apinfo = aplist[current_ap_index]
        wifi.sta.config(apinfo.ssid,"31415926",apinfo.bssid)
        wifi.sta.connect();step_connecting=0
        print('try..'..apinfo.ssid)
        current_ap_index = current_ap_index+1
        if(current_ap_index > table.getn(aplist)) then
            current_ap_index = 1;
        end
    end


    function listap(t)

        for bssid,v in pairs(t) do
            local ssid, rssi, authmode, channel = string.match(v, "(.*),(-?%d+),(%d+),(%d+)")
            if( tonumber(rssi) > -50) then
                print(string.format("%32.s",ssid).."\t"..bssid.."\t  "..rssi.."\t\t"..authmode.."\t\t\t"..channel)
                table.insert(aplist,1,{bssid=bssid,ssid=ssid})
            end
        end


        function startConnectHelper()


            function connect_helper()
                print('do connecting helper...')

                local status= wifi.sta.status();
                if (status == wifi.STA_CONNECTING) then
                    if(step_connecting == 0) then
                        step_connecting = 1;
                    else
                        wifi.sta.disconnect()
                        step_connecting = 0;
                    end

                end
                tmr.alarm(6,10000,0,connect_helper)
            end

            tmr.stop(6)
            wifi.sta.disconnect()
            connectNextAp()
            connect_helper()
        end

        startConnectHelper()

    end
    wifi.sta.getap(1, listap)
end





