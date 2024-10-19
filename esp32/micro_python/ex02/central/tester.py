import threading
import asyncio
from bleak import BleakScanner, BleakClient


class bleThread(threading.Thread):
    def __init__(self, mac_address):
        super().__init__()
        self.mac_address = mac_address
        self.termination_requested = False
        self.characteristic_uuid = "12345678-1234-5678-1234-56789abcdef1"
        
    def run(self):
        loop = asyncio.new_event_loop()  # 새로운 이벤트 루프 생성
        asyncio.set_event_loop(loop)  # 현재 쓰레드의 이벤트 루프로 설정
        loop.run_until_complete(self.run_ble())
        print("bleThread terminated")
        
    async def run_ble(self):
        self.client = BleakClient(self.mac_address)
        async with self.client:
            if self.client.is_connected : 
                print("Connected!")
                
            while self.termination_requested == False :
                await asyncio.sleep(1)
                
                
            await self.client.disconnect()
            print(f"Disconnected from {self.mac_address}")
            
    async def _send_data(self):
        if self.client.is_connected:
            # await self.client.write_gatt_char(self.characteristic_uuid, data.encode())
            # print(f"Sent {data} to {self.mac_address}")
            # # 캐릭터리스틱에 'i' 쓰기
            await self.client.write_gatt_char(self.characteristic_uuid, b'i')
            print("Sent 'i'")
            # 응답 읽기
            response = await self.client.read_gatt_char(self.characteristic_uuid)
            print(f"Received response: {response.decode('utf-8')}")
        else:
            print(f"Cannot send data to {self.mac_address} because it is not connected")
    def send_data(self):
        loop = asyncio.get_event_loop()
        loop.run_until_complete(self._send_data())
            
    def terminate(self):
        self.termination_requested = True

def bleScan():
    async def run():
        scanner = BleakScanner()
        devices = await scanner.discover()
        # target_name = "ESP32_YOUR_NAME" # 찾고자 하는 광고 이름

        for device in devices:
            if device.name is not None:
                print(f"Found device with name {device.name}, MAC address: {device.address}")
                # 여기서 device.address를 사용하여 연결할 수 있습니다.                
    print("Scanning...")
    loop = asyncio.get_event_loop()    
    loop.run_until_complete(run())
    print("Scan done.")

connected_client = None

while True:
    command = input("Enter command (help, scan, connect, send, exit): ")
    
    if command == "":  # 빈 문자열이면 무시
        pass    
    elif command == "help":
        print("help: Show help")
        print("scan: Scan for devices")
        print("connect [mac_address]: Connect to device")
        print("send [message]: Send message")
        print("exit: Exit program")    
    elif command == "scan":
        bleScan()
    elif command.startswith("connect"):
        mac_address = command.split(" ")[1]
        print(f"Connecting to {mac_address}...")
        thread = bleThread(mac_address)
        thread.start()
        connected_client = thread
    elif command.startswith("disconnect"):
        mac_address = command.split(" ")[1]
        print(f"Disconnecting from {mac_address}...")
        connected_client.terminate()
        connected_client = None
    elif command.startswith("send"):
        connected_client.send_data()
    elif command == "exit":
        if connected_client != None:
            connected_client.terminate()
        break    
    else:
        print("Unknown command")
