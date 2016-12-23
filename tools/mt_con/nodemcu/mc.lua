gpio.mode(1,gpio.OUTPUT) --5
gpio.mode(2,gpio.OUTPUT) --4 

--pwm.setup(1, 500, 512)
pwm.setup(2, 100, 0)
pwm.start(2)

function lf_mt(speed) 
    if speed > 0 then gpio.write(1,0) pwm.setduty(2, speed)
    elseif speed < 0 then gpio.write(1,1) pwm.setduty(2, 1023 + speed)
    else gpio.write(1,0) pwm.stop(2)
    end
end

gpio.mode(5,gpio.OUTPUT) --14
gpio.mode(6,gpio.OUTPUT) --12

pwm.setup(6, 100, 0)
pwm.start(6)

function rt_mt(speed) 
    if speed > 0 then gpio.write(5,0) pwm.setduty(6, speed)
    elseif speed < 0 then gpio.write(5,1) pwm.setduty(6, 1023 + speed)
    else gpio.write(5,0) pwm.stop(6)
    end
end