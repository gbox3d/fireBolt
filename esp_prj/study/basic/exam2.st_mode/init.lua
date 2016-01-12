wifi.sleeptype(wifi.NONE_SLEEP)
wifi.setmode(wifi.STATION)wifi.sta.config("esptest","")
wifi.sta.setip({ip="192.168.9.20",netmask="255.255.255.0",gateway=""})
wifi.sta.connect()
print("connect :" + wifi.sta.getip())

