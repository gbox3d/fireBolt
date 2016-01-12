pin = 2; -- gpio4
index = 1;
count = 0;
duty_table = {60,102,160,204,256 }

pwm.setup(pin,100,152)
pwm.start(pin)

tmr.alarm(0,500,1,function()
    print( "duty table index ".. index .. " ," .. duty_table[index])
    pwm.setduty(pin,duty_table[index])
    index = index+1;
    if index > table.getn(duty_table) then
        index = 1
        count = count+1
    end
    if count > 3 then
        tmr.stop(0)
        tmr.alarm(0,500,0,function() pwm.stop(pin) pwm.close(pin) end)
    end
end)

--tmr.stop(0)