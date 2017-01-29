print('load extention')
--dofile("")

function ext_main(delay) 
    print("extention main start") 
    tmr.create():alarm(delay,tmr.ALARM_SINGLE, 
    function() 
    --dofile("")
    
    end)
end
