wifi.sleeptype(wifi.NONE_SLEEP)
wifi.setmode(wifi.STATION)
wifi.sta.setip({ip = "192.168.9.20",netmask = "255.255.255.0",gateway=""})
my_ssid = "GUNPOWER_PI_1"
wifi.sta.config(my_ssid,"")
wifi.sta.connect()