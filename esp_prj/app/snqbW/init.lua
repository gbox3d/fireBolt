dofile('async_safe_sender.lua')
dofile('stub.lua')
dofile('app.lua')
dofile("ext.lua")

boot_status = {}
gpio.mode(0,1)gpio.write(0,0)

function save_BootStatus() file.open("status.json", 'w') file.write(cjson.encode(boot_status)) file.close() end
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
    else print( "prev process is "..boot_status.process.." and now start system..") boot_status.process = "startup" save_BootStatus() system_start_up() end

else
    boot_status.process = "startup"
    save_BootStatus();
    print( "first bootup") system_start_up()
end




