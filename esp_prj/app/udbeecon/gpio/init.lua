dofile("config.lua")
dofile('app.lua')

--5(14),6(12),7(13),0(16)
gpio.mode(0,gpio.OUTPUT)
gpio.mode(5,gpio.OUTPUT)
gpio.mode(6,gpio.OUTPUT)
gpio.mode(7,gpio.OUTPUT)
gpio.write(0,0);



device_id = "udbCon"..node.chipid();
wifi.setmode(wifi.SOFTAP)
wifi.ap.config({ssid=device_id,pwd=nil})

--wifi.setphymode(wifi.PHYMODE_B)
wifi.setphymode(wifi.PHYMODE_G)
wifi.sleeptype(wifi.NONE_SLEEP)

start_UdpController(device_id,"192.168.4.1",app_config.port)

print('start udbeeCon stepper version(GPIO) : 0.0.1')
