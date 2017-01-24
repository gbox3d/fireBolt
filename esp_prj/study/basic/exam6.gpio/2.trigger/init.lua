--
-- Created by IntelliJ IDEA.
-- User: gunpower
-- Date: 2016. 10. 13.
-- Time: PM 4:06
-- To change this template use File | Settings | File Templates.
--

do
    local pin = 1
    local pulse1 = tmr.now()
    gpio.mode(pin,gpio.INT)

    function trigcb()
        if  (tmr.now() - pulse1) > 1000*300 then --잡음제거
            pulse1=tmr.now()
            print("down")
        end

    end
    gpio.trig(pin, "up",trigcb) -- up -> down 으로 상태가 변할때  (눌렀을때 콜)
end