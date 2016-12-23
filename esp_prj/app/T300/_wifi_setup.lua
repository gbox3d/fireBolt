
--wifi.sleeptype(wifi.NONE_SLEEP)
--wifi.setphymode(wifi.PHYMODE_B)
--wifi.setmode(wifi.SOFTAP)
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
--ap 모드에서 기본 주소는 192.168.4.1 이 됨

--wifi.ap.config({ssid="T300AP-rd1",pwd="123456789"})
--wifi.ap.setip({ip="192.168.9.1",netmask="255.255.255.0",gateway="192.168.9.1"})

wifi.setmode(wifi.SOFTAP)
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
wifi.ap.config({ssid="esptest",pwd="123456789"})
wifi.ap.setip({ip="192.168.9.1",netmask="255.255.255.0",gateway="192.168.9.1"})
print("ap mode test")

--wifi.sleeptype(wifi.NONE_SLEEP)
--wifi.setphymode(wifi.PHYMODE_B)
--wifi.setmode(wifi.STATION)
--wifi.sta.setip({ip = "192.168.9.20",netmask = "255.255.255.0",gateway=""})
--wifi.sta.config("GUNPIE-1","123456789")
--wifi.sta.connect()

