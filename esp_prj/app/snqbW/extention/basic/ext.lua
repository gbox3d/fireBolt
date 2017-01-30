print('load extention')
--dofile("")

local __doit = function()

    gpio.mode(0,1)
    gpio.write(0,0)
    --network latancy check
    tmr.create():alarm(1000,tmr.ALARM_AUTO,function () 
    app_config.latancy = (tmr.now()-last_nt_tick) 
    if app_config.latancy <0 then app_config.latancy=0 last_nt_tick =tmr.now() end
    print(app_config.latancy )
    if app_config.latancy > 500000 then
        gpio.write(0,1)
    else
        gpio.write(0,0)
    end
    end)

end

function ext_main(delay) 
    print("extention main start") 
    
    tmr.create():alarm(delay,tmr.ALARM_SINGLE, 
        function() 
            local __ps = boot_status.process;
            boot_status.process = 'stub_check'            
            save_BootStatus();    
    
    --print(boot_status.process)
            __doit();
            
    --dofile("")

            boot_status.process = __ps
            save_BootStatus();
    
        end)
end
