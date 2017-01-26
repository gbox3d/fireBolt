gpio.mode(1,1)
gpio.mode(2,1)
gpio.mode(6,1)
gpio.mode(7,1)

gpio.write(1,0)
gpio.write(6,0)
pwm.setup(2,100,0)
pwm.setup(7,100,0)

 rt ={type="rs",id=0} udp_safe_sender(cjson.encode(rt),2012,"192.168.9.6") 




