print('load extention')
--dofile("")

function ext_main(delay) 
    print("extention main start") 
    tmr.create():alarm(delay,tmr.ALARM_SINGLE, 
    function() 
    local prev_status = boot_status.process;
    boot_status.process = 'stub_check'
    save_BootStatus();
    --dofile("")

    boot_status.process = prev_status
    save_BootStatus();
    
    end)
end
