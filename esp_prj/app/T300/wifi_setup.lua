wifi.setmode(wifi.SOFTAP)wifi.ap.config({ssid="esptest",pwd="123456789"})wifi.ap.setip({ip="192.168.9.1",netmask="255.255.255.0",gateway="192.168.9.1"})print("ap mode test")
