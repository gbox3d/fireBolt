#%%
import sys
import asyncio
from bleak import BleakClient
import yaml
import struct
# import logging

# logging.basicConfig(level=logging.DEBUG)

CHECK_CODE = 230815

# Read from config file
with open("config.yaml", 'r') as stream:
    try:
        config = yaml.safe_load(stream)
        address = config['mac']
        SERVICE_UUID = config['service_uuid']
        CHARACTERISTIC_UUID = config['characteristic_uuid']
        print("Address:", address)
        print("Service UUID:", SERVICE_UUID)
        print("Characteristic UUID:", CHARACTERISTIC_UUID)
    except yaml.YAMLError as exc:
        print(exc)

async def write_command(client, cmd, params):
    packet = struct.pack("<IB3B", CHECK_CODE, cmd, params[0], params[1], params[2])
    await client.write_gatt_char(CHARACTERISTIC_UUID, packet)
    print(f"Written command {cmd} with params {params} to characteristic {CHARACTERISTIC_UUID}")

# Notify callback function
def notification_handler(sender, data):
    # This function will be called whenever the characteristic is updated
    if len(data) == 8:
        response_packet = struct.unpack("<LB3B", data) # Unpacking the received packetq
        print(f"Received data: {response_packet}")
        # check_code, cmd, parms = response_packet
        # if check_code == CHECK_CODE:
        #     print(f"Received command {cmd} with parameters {parms}")
    else:
        print(f"Received data: {data}")

def parse_response_header(response_data):
    response_packet_format = "<I B 3B" # uint32_t, uint8_t, 3x uint8_t
    response_packet = struct.unpack(response_packet_format, response_data)

    check_code, cmd, parm1, parm2, parm3 = response_packet

    if check_code != CHECK_CODE:
        print("Invalid check code in response!")
        return None

    print(f"Command: {cmd}, Parameters: {parm1}, {parm2}, {parm3}")

    return response_packet
 
async def run(address, loop):
    async with BleakClient(address, loop=loop) as client:
        
        # Subscribe to the characteristic for notifications    
        try:
            await client.start_notify(CHARACTERISTIC_UUID, notification_handler)
            # await asyncio.sleep(1) # Adding a delay to make sure notifications are ready
            # response_data = await client.read_gatt_char(CHARACTERISTIC_UUID)
            # parse_response_header(response_data)
        except Exception as e:
            print("Error starting notification:", e)
        
        # Reading the welcome message from the characteristic
        # welcome_message = await client.read_gatt_char(CHARACTERISTIC_UUID)
        # print(f"Received welcome message: {welcome_message.decode('utf-8')}")
        # Adding a delay to ensure the ESP32 has processed the message
        # await asyncio.sleep(3) # 1-second delay
        
        while True:
            cmd_input = input("Enter command: ")
            _cmd = cmd_input[0]

            if _cmd == 'q':
                
                break
            elif _cmd == 'n': #notify test
                await write_command(client, 0x04, [0x00, 0x00, 0x00])
                response_data = await client.read_gatt_char(CHARACTERISTIC_UUID)
                parse_response_header(response_data)
            elif _cmd == 'r':
                await write_command(client, 0x01, [0x99, 0x00, 0x00])
                response_data = await client.read_gatt_char(CHARACTERISTIC_UUID)
                parse_response_header(response_data) 
            elif _cmd == 'o':
                led_index = int(cmd_input[1]) # Extracting the LED index
                await write_command(client, 0x02, [led_index, 0x00, 0x00])
            elif _cmd == 'f':
                led_index = int(cmd_input[1]) # Extracting the LED index
                await write_command(client, 0x03, [led_index, 0x00, 0x00])
            
         # Make sure to stop the notification when done
        await client.stop_notify(CHARACTERISTIC_UUID)
        #disconnect
        await client.disconnect()

#%%
loop = asyncio.get_event_loop()
loop.run_until_complete(run(address, loop))
# %%
