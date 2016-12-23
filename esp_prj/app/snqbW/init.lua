majorVer, minorVer, devVer, chipid, flashid, flashsize, flashmode, flashspeed = node.info()
lua_botloader_version = "1.0"
print("\nfind init.lua now startup lua bootloader " .. lua_botloader_version)


if file.exists("config.json") then
    file.open("config.json", 'r') local config_file = file.read() file.close()
    app_config = cjson.decode(config_file)
else
    print("find config.lua")
    dofile("config.lua")
    file.open("config.json", 'w') file.write(cjson.encode(app_config)) file.close()
end

OnStart = function(...) end
dofile('app.lua')
dofile("ext.lua")

function system_start_up()
    --node.restart();
    --wifi.setphymode(wifi.PHYMODE_B)
    --wifi.setphymode(wifi.PHYMODE_N)
    wifi.setphymode(wifi.PHYMODE_G)
    wifi.sleeptype(wifi.NONE_SLEEP)

    OnStart();

    function setupAP()

        wifi.sta.autoconnect(0)
        wifi.sta.disconnect()

        wifi.setmode(wifi.SOFTAP)

        --패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
        --wifi.ap.config({ssid="esptest",pwd="123456789"})
        -- nil 을 주면 패스워드 설정안해도됨
        wifi.ap.config({ssid="snqbW"..chipid,pwd=nil})
        wifi.ap.setip({ip="192.168.9.1",netmask="255.255.255.0",gateway="192.168.9.1"})
        print("start ap mode")

        startup()
    end

    function setupSTA(callback)

        wifi.setmode(wifi.STATION)
        wifi.sta.autoconnect(1)
        wifi.sta.disconnect()

        wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE") callback(wifi.STA_IDLE) end)
        wifi.sta.eventMonReg(wifi.STA_CONNECTING, function() print("STATION_CONNECTING") callback(wifi.STA_CONNECTING) end)
        wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") callback(wifi.STA_WRONGPWD) end)
        wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") callback(wifi.STA_APNOTFOUND) end)
        wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL") callback(wifi.STA_FAIL) end)
        wifi.sta.eventMonReg(wifi.STA_GOTIP, function()  callback(wifi.STA_GOTIP) end)

        wifi.sta.eventMonStart()
        wifi.sta.config(app_config.ssid,app_config.passwd,1)
        if app_config.dhcp then

        else
            wifi.sta.setip({
                ip = app_config.ip[1].."."..app_config.ip[2].."."..app_config.ip[3].."."..app_config.ip[4],
                netmask = "255.255.255.0",
                gateway= app_config.ip[1].."."..app_config.ip[2].."."..app_config.ip[3]..".1"
            })

        end
        wifi.sta.connect()

    end

    if app_config.apmode then
        setupAP()
        startUdpCast();
        boot_status.process = 'APOK'
        save_BootStatus();
    else

        setupSTA(
            function(evt)
                if evt ==  wifi.STA_GOTIP then
                    print("STATION_GOT_IP")
                    print(wifi.sta.getip());
                    print("dhcp mode");
                    local strip = wifi.sta.getip();
                    --print(strip)
                    if app_config.dhcp then app_config.ip = {string.match(strip,"(%d+).(%d+).(%d+).(%d+)" )} end
                    startup()
                    startUdpCast();
                    boot_status.process = 'STOK'
                    save_BootStatus();
                elseif evt ==  wifi.STA_FAIL or
                        evt ==  wifi.STA_APNOTFOUND or
                        evt ==  wifi.STA_WRONGPWD then
                    wifi.sta.disconnect() 
                    setupAP()
                    startUdpCast();
                    boot_status.process = 'APOK'
                    save_BootStatus(); end
            end)

    end


end

boot_status = {}
gpio.mode(0,1)gpio.write(0,0) --gpio 16
function save_BootStatus() file.open("status.json", 'w') file.write(cjson.encode(boot_status)) file.close() end
if file.exists("status.json") then
    file.open("status.json", 'r') local data = file.read() file.close()
    if data == nil then
        boot_status.process = "startup"
        save_BootStatus();
        print( "first bootup") system_start_up()
    else
        boot_status = cjson.decode(data)
        if boot_status.process == "startup" then --이전상태가 startup 상태에서 최종마무리되었다면...
            print("system abnormal")gpio.write(0,1)
            boot_status.process = "nook" save_BootStatus()
        else print( "prev process is "..boot_status.process.." and now start system..") boot_status.process = "startup" save_BootStatus() system_start_up() end
    end
else
    boot_status.process = "startup"
    save_BootStatus();
    print( "first bootup") system_start_up()
end




