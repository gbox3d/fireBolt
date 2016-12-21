gpio.mode(1,gpio.OUTPUT)
gpio.mode(2,gpio.OUTPUT)
--pwm.setup(1, 500, 512)
pwm.setup(2, 100, 0)
pwm.start(2)

function lf_mt(speed) 
    if speed > 0 then gpio.write(1,0) pwm.setduty(2, speed)
    elseif speed < 0 then gpio.write(1,1) pwm.setduty(2, 1023 + speed)
    else gpio.write(1,0) pwm.stop(2)
    end
end

