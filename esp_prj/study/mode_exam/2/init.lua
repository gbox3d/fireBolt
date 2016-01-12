--wifi.sleeptype(wifi.NONE_SLEEP)
--wifi.setmode(wifi.STATION)
wifi.setmode(wifi.STATIONAP)
--wifi.setmode(wifi.SOFTAP)
-- ap setup
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
wifi.ap.config({ssid="esptest2",pwd="12345678"})

wifi.sta.setip({ip = "192.168.42.22",netmask = "255.255.255.0",gateway=""})
wifi.sta.config("CAMPIE-1","123456789")
wifi.sta.connect()