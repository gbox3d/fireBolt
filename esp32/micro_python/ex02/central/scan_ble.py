import asyncio
from bleak import BleakScanner
from bleak import BleakScanner, BleakClient

def scan_BLE() :
    
    async def print_services_and_characteristics(device):
        client = BleakClient(device.address)
        try:
            await client.connect()
                        
            for service in client.services:
                print(f"  - Service UUID: {service.uuid}")
                for characteristic in service.characteristics:
                    print(f"    - Characteristic UUID: {characteristic.uuid}") # 캐릭터리스틱 UUID 출력                
                print(f"liting done - Service UUID: {service.uuid}")
            print(f"scan done address : {device.address}")
        except Exception as e:
            print(e)
        finally:
            await client.disconnect()  # 연결 끊기
        
    async def run():
        scanner = BleakScanner()
        devices = await scanner.discover()
        # target_name = "ESP32_YOUR_NAME" # 찾고자 하는 광고 이름

        for device in devices:
            if device.name is not None:
                print(f"Found device with name {device.name}, MAC address: {device.address}")                
                
                # 디바이스와 연결하고 uuid를 읽어온다.
                await print_services_and_characteristics(device)
                
                
                
    print("Scanning...")

    loop = asyncio.get_event_loop()
    loop.run_until_complete(run())
    print("Scan done.")
    
scan_BLE()


