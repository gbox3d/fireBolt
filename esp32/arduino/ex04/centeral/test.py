import asyncio
import yaml
from bleak import BleakClient

async def print_welcome_message(mac_address, characteristic_uuid):
    client = BleakClient(mac_address)
    try:
        await client.connect()
        if client.is_connected:
            print("Connected to device:", mac_address)
            
            # 환영 메시지 읽기
            welcome_message = await client.read_gatt_char(characteristic_uuid)
            print("Welcome message:", welcome_message.decode('utf-8'))
        else:
            print("Failed to connect to device:", mac_address)
    finally:
        await client.disconnect()
        print("Disconnected from device:", mac_address)

# YAML 파일에서 설정 읽기
with open('config.yaml', 'r') as file:
    config = yaml.safe_load(file)
    mac_address = config['mac']
    characteristic_uuid = config['char_uuid']

loop = asyncio.get_event_loop()
loop.run_until_complete(print_welcome_message(mac_address, characteristic_uuid))
