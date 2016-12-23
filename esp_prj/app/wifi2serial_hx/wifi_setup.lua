wifi.setmode(wifi.STATIONAP)

-- ap setup
cfg={}
cfg.ssid="balance-sensor"
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
cfg.pwd="12345678"
wifi.ap.config(cfg)
--ap 모드에서 기본 주소는 192.168.4.1 이 됨

cfg = {
    ip = "192.168.9.108",
    netmask = "255.255.255.0",
    gateway="192.168.9.177"
}
--wifi.sta.setip(cfg)
wifi.sta.connect()
wifi.sta.config("camtic-sensor","123456789")

