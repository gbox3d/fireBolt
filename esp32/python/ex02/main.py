from ubluetooth import BLE, UUID, FLAG_READ, FLAG_WRITE
import ubinascii
import network
from machine import Pin

led = Pin(5, Pin.OUT) # GPIO 2에 연결된 LED
led.off()

def ble_irq(event, data):
    if event == 3: # 쓰기 이벤트
        conn_handle, attr_handle = data
        value = ble.gatts_read(attr_handle)
        if value == b'i':
            ble.gatts_write(attr_handle, b'hello')
            print("Received 'i', responded with 'hello'")
    elif event == 1: # 연결 이벤트
        print("Connected")
        led.on()
    elif event == 2: # 연결 해제 이벤트
        print("Disconnected, restarting advertising")
        start_advertising()
        led.off()

ble = BLE()
ble.active(True)
ble.irq(ble_irq)

# 서비스 및 특성 정의
service_uuid = UUID('12345678-1234-5678-1234-56789abcdef0')
characteristic_uuid = UUID('12345678-1234-5678-1234-56789abcdef1')
characteristic = (characteristic_uuid, FLAG_READ | FLAG_WRITE)
service = (service_uuid, (characteristic,), )
services = (service, )
((characteristic_handle,),) = ble.gatts_register_services(services)

# MAC 주소 가져오기
mac = network.WLAN().config('mac')
mac_address = ubinascii.hexlify(mac).decode('utf-8')


def start_advertising():
    # 로컬 이름 설정
    local_name = f'bsqble_{mac_address}'
    adv_data = bytearray([len(local_name) + 1, 0x09]) + local_name.encode('utf-8')

    # 광고 시작
    ble.gap_advertise(100, adv_data)
    print("Advertising as ESP32, waiting for connection..." + local_name)
    
start_advertising()

# adv_name = "bsq_" + mac_address 
# adv_data = ubinascii.hexlify(adv_name.encode('utf-8'))

# # 로컬 이름 설정
# local_name = f'bsqble_{mac_address}'
# adv_data = bytearray([len(local_name) + 1, 0x09]) + local_name.encode('utf-8')

# # 광고 시작
# ble.gap_advertise(100, adv_data)
# print("Advertising as ESP32, waiting for connection..." + local_name)
