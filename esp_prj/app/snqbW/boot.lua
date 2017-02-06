-------------- apis -----------------------------------
majorVer, minorVer, devVer, chipid, flashid, flashsize, flashmode, flashspeed = node.info()
boot_status = {} -- manage system values & flags

--버퍼에 있는대로 모아서 보내기
function AsyncSender_Safe_udp(option)
    local test_set = false;
    local tempBuf = "";
    local _timer = tmr.create()
    return function(data,_port,_ip)
        local socket = option.getsocket()
        function _send(data)
            --print("buf : " .. data)
            if test_set == true then tempBuf = tempBuf..data --print(tempBuf)
            else
                local _data = tempBuf..data;tempBuf=""
                if _data:len()>0 then
                    test_set=true
                    --print("send :  " .._data)                    
                    socket:send(_port,_ip,_data,
                        function()
                            --need cool time ?
                            --print("sent" .._data)
                            _timer:alarm(1,tmr.ALARM_SINGLE,function()
                                test_set=false
                                if tempBuf:len()>0 then
                                    _send("") end
                            end)
                        end
                    )
                end
            end
        end
        if socket~=nil then _send(data) end
    end --function
end

function save_BootStatus() file.open("status.json", 'w') file.write(cjson.encode(boot_status)) file.close() end
------------------------------------


function system_start_up()

    dofile("ext.lua")

    if file.exists("config.json") then
        file.open("config.json", 'r') local config_file = file.read() file.close()
        app_config = cjson.decode(config_file)
    else
        print("find config.lua")
        dofile("config.lua")
        file.open("config.json", 'w') file.write(cjson.encode(app_config)) file.close()
    end
    --node.restart();
    --wifi.setphymode(wifi.PHYMODE_B)
    --wifi.setphymode(wifi.PHYMODE_N)
    wifi.setphymode(wifi.PHYMODE_G)
    wifi.sleeptype(wifi.NONE_SLEEP)

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

-- wifi mode setup
    if app_config.apmode then
        setupAP()
        if boot_status.process == "stub_check" then
            boot_status.process = 'APOK'
            boot_status.mode = 'check'
            save_BootStatus();
        else
            boot_status.process = 'APOK'
            boot_status.mode = 'normal'
            save_BootStatus();
            ext_main(1000);
        end
    else
        setupSTA(
            function(evt)
                if evt ==  wifi.STA_GOTIP then
                    print("STATION_GOT_IP")
                    print(wifi.sta.getip());
                    local strip = wifi.sta.getip();
                    --print(strip)
                    if app_config.dhcp then 
                        print("dhcp mode")app_config.ip = {string.match(strip,"(%d+).(%d+).(%d+).(%d+)" )} 
                    else
                        print("static ip mode")
                    end
                    
                    startup()
                    
                    if boot_status.process == "stub_check" then
                        boot_status.process = 'STOK'
                        boot_status.mode = 'check'
                        save_BootStatus();
                        
                    else
                        boot_status.process = 'STOK'
                        boot_status.mode = 'normal'
                        save_BootStatus();
                        ext_main(1000);
                    end

                    print("mode:"..boot_status.mode)
                    

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


--------------------App -----------------
app_version='0.0.3'
app_status = {fsm=0}; -- manage runtime values & flags

function startup()

    print("App version " .. app_version .. " start..")
    timerid_Udpcaster = tmr.create();
    network_latency = 0;
    last_nt_tick = tmr.now();

    master_socket = net.createUDPSocket()

    local ip = app_config.ip;
    local broad_ip = ip[1] .. ".".. ip[2].."." ..ip[3] .. ".255" 
    
    print("broad cast : " .. broad_ip .. "," .. app_config.bc_port)

    udp_safe_sender = AsyncSender_Safe_udp({getsocket = function() return master_socket end})
    --udp_safe_sender = function(port,ip,data) master_socket:send(ip,port,data) end

    packet_dic = {
        default = function() local rt = {result = 'nocmd'} udp_server:send(cjson.encode(rt))  end
    }

    packet_dic["eval"] = function(packet) 
        local _f=loadstring(packet.code) 
        if(_f) then _f() 
        else 
            print("script err " .. packet.code) 
        end 
    end    

    function processRecv(s,c,_port,_ip)
        last_nt_tick = tmr.now()        
        --print(_ip .. "," .. _port)
        if c:byte(1,1) == 123 then -- check '{}'
            local packet = cjson.decode(c)
            --print(c)
            if packet.cmd ~= nil then
                if packet_dic[packet.cmd] ~= nil then
                    packet_dic[packet.cmd](packet)
                else
                    print("unknown packet")
                    --print(c)
                    --local rt = {result="nocmd"} udp_server:send(cjson.encode(rt))
                end
            end

        end 

    end
    master_socket:on("receive",processRecv)
    master_socket:listen(app_config.data_port)

    startUdpCast = function()    
        --tmr.alarm(timerid_Udpcaster,app_config.cast_delay,tmr.ALARM_AUTO,
        print("cast delay :" .. app_config.cast_delay)
        timerid_Udpcaster:alarm(app_config.cast_delay,tmr.ALARM_AUTO,
            function()
                local delta = tmr.now() - last_nt_tick
                --print(delta)
                --if delta < 0 then delta = delta + 2147483647 end -- proposed because of delta rolling over, https://github.com/hackhitchin/esp8266-co-uk/issues/2
                if delta < 0 then delta = 0 last_nt_tick = tmr.now() end
                network_latency = delta;
                if(network_latency > 5000000) then -- delay over 5 sec then wakeup broadcasting
                    master_socket:send(app_config.bc_port,broad_ip,cjson.encode({sk=1,did=0,cid=chipid,type="bc",aps=app_status}))
                    --master_socket:send(cjson.encode({sk=1,did=0,cid=chipid,type="bc",aps=app_status}))
                end
            end)
    end

    stopUdpCast = function() timerid_Udpcaster:stop() end
    startUdpCast();    
end

--------------------------------------------------

if file.exists("status.json") then
    file.open("status.json", 'r') local data = file.read() file.close()
    if data == nil then boot_status.process = "nook"
    else
        boot_status = cjson.decode(data)
        
    end
    --이전실행상태 검사 
    if boot_status.process == "startup" or boot_status.process == "repair" then
        print("repair mode")gpio.write(0,1)
        boot_status.process = "nook" save_BootStatus()
    elseif boot_status.process == "stub_check"  then
        --boot_status.mode = 'check'
        system_start_up()
    else print( "prev process is "..boot_status.process.." and now start system..") 
        boot_status.process = "startup" 
        save_BootStatus() 
        system_start_up() end

else
    boot_status.process = "startup"
    save_BootStatus();
    print( "first bootup") system_start_up()
end



