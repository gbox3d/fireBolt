wifi.setmode(wifi.STATION)

cfg = {
    ip = "192.168.9.108",
    netmask = "255.255.255.0",
    gateway="192.168.9.177"
}
wifi.sta.setip(cfg)
wifi.sta.connect()
wifi.sta.config("GUNPOWER_PI_1","")