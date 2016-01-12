print('hello')
gpio.mode(0,1)
gpio.write(0,0)
blink = 1
stop = false
tmr.alarm(0,1000,1,function() 
    if blink==0 then blink=1 else blink=0 end
    gpio.write(0,blink)
    if stop==true then tmr.stop(0) end
end)
