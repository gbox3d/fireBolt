import asyncio
from bleak import BleakClient
import yaml

# YAML 파일에서 설정 읽기
with open('config.yaml', 'r') as file:
    config = yaml.safe_load(file)
    mac_address = config['mac']
    characteristic_uuid = config['char_uuid']
    
async def send_hello_message():
    client = BleakClient(mac_address)
    try:
        # 서버에 연결합니다.
        await client.connect()
        if client.is_connected:
            print("Connected to device:", mac_address)

            # "hello" 문자열을 보냅니다.
            message = "hello"
            await client.write_gatt_char(characteristic_uuid, bytearray(message, 'utf-8'))
            print("Message sent:", message)
            
            # 응답을 읽습니다.
            response = await client.read_gatt_char(characteristic_uuid)
            print("Received response:", response.decode('utf-8'))
            
        else:
            print("Failed to connect to device:", mac_address)
    finally:
        # 연결을 끊습니다.
        await client.disconnect()
        print("Disconnected from device:", mac_address)

loop = asyncio.get_event_loop()
loop.run_until_complete(send_hello_message())
