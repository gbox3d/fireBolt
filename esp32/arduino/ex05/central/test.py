import asyncio
import yaml
import struct
from bleak import BleakClient

async def connect(mac_address,characteristic_uuid):
    global client
    client = BleakClient(mac_address)
    try:
        await client.connect()
        if client.is_connected:
            print("Connected to device:", mac_address)
            # 환영 메시지 읽기
            welcome_message = await client.read_gatt_char(characteristic_uuid)
            print("Welcome message:", welcome_message.decode('utf-8'))     
            return client
        else:
            print("Failed to connect to device:", mac_address)
    except Exception as e:
        print(e)
        
async def disconnect(client):
    # global client
    await client.disconnect()
    print("Disconnected from device:", mac_address)

# async def led_on(client,characteristic_uuid,led_index):
#     check_code = 20230815
#     cmd = 0x02
#     parms = [led_index,0,0]
#     _req_pkt = struct.pack("<LB3B", check_code, cmd, *parms)
#     await client.write_gatt_char(characteristic_uuid, _req_pkt)
#     response = await client.read_gatt_char(characteristic_uuid)
#     response_check_code, response_cmd, *response_parms = struct.unpack("<I B 3B", response)
#     print(response_check_code, response_cmd, response_parms)
#     if response_cmd == 0x02:
#         print("LED ON ok")
#     # print("Received response:")

# async def led_off(client,characteristic_uuid,led_index):
#     check_code = 20230815
#     cmd = 0x03
#     parms = [led_index,0,0]
#     _req_pkt = struct.pack("<LB3B", check_code, cmd, *parms)
#     await client.write_gatt_char(characteristic_uuid, _req_pkt)
#     response = await client.read_gatt_char(characteristic_uuid)
#     response_check_code, response_cmd, *response_parms = struct.unpack("<I B 3B", response)
#     if response_cmd == 0x03:
#         print("LED OFF ok")
    
    
async def echo(client,characteristic_uuid, check_code, cmd, parms):
    # global client
    # 에코 메시지 생성
    check_code = 20230815
    cmd = 0x01
    parms = [0x11, 0x20, 0x30]
    echo_message = struct.pack("<LB3B", check_code, cmd, *parms)
    
    # 에코 메시지 쓰기
    await client.write_gatt_char(characteristic_uuid, echo_message)
    
    # 응답 읽기
    response = await client.read_gatt_char(characteristic_uuid)
    response_check_code, response_cmd, *response_parms = struct.unpack("<I B 3B", response)
    
    print("Received response:")
    print("Check code:", response_check_code)
    print("Command:", response_cmd)
    print("Parameters:", response_parms)
    
async def test1(client,characteristic_uuid):
    await client.write_gatt_char(characteristic_uuid, "hello world".encode('utf-8'))
    
    

# YAML 파일에서 설정 읽기
with open('config.yaml', 'r') as file:
    config = yaml.safe_load(file)
    mac_address = config['mac']
    characteristic_uuid = config['char_uuid']

loop = asyncio.get_event_loop()
# loop.run_until_complete(print_welcome_message(mac_address, characteristic_uuid))

client = None
while True:
    # loop.run_until_complete(print_welcome_message(mac_address, characteristic_uuid))
    # print("Press Ctrl+C to quit")
    
    try:
        _cmd = input("Enter command: ")
        
        if _cmd == 'quit':
            if client is not None:
                loop.run_until_complete(disconnect(client))
            break
        elif _cmd == 't':
            loop.run_until_complete(test1(client,characteristic_uuid))
        elif _cmd == 'con':
            clinet = loop.run_until_complete(connect(mac_address, characteristic_uuid))
        elif _cmd == 'dcon':
            loop.run_until_complete(disconnect(client))
            client = None
        # elif _cmd == 'on':
            # loop.run_until_complete(led_on(client,characteristic_uuid,1))
        # elif _cmd == 'off':
            # loop.run_until_complete(led_off(client,characteristic_uuid,1))
        elif _cmd == 'echo':
            check_code = 20230815
            cmd = 0x01
            parms = [0x10, 0x20, 0x30]
            loop.run_until_complete(echo(client, characteristic_uuid, check_code, cmd, parms))
        
        # loop.run_forever()
    except KeyboardInterrupt:
        break
        
loop.close()
