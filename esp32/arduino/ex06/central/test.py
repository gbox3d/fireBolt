#%%
import asyncio
from bleak import BleakClient
import yaml
import struct

CHECK_CODE = 230815

# Read from config file
with open("config.yaml", 'r') as stream:
    try:
        config = yaml.safe_load(stream)
        address = config['mac']
        SERVICE_UUID = config['service_uuid']
        CHARACTERISTIC_UUID = config['characteristic_uuid']
        AP_SSID = config['ap_ssid']
        AP_PASSWORD = config['ap_password']
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
    if len(data) >= 8:
        #head size is 8 bytes
        response_packet = parse_response_header(data[:8])
        print(f"Received data: {response_packet}")        
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
        except Exception as e:
            print("Error starting notification:", e)        
        
        while True:
            cmd_input = input("Enter command: ")
            _cmd = cmd_input[0]

            if _cmd == 'q':                
                break
            elif _cmd == 'w':
                _config_data = struct.pack("<16s16sI B", AP_SSID.encode(), AP_PASSWORD.encode(), 100, 1 )
                #27 bytes padding
                _config_data += struct.pack("<27s", b'')                
                header = struct.pack("<I B 3B", CHECK_CODE, 0x05, 0, 0, 0) # 0x05 is the command for writing config data
                packet = header + _config_data
                await client.write_gatt_char(CHARACTERISTIC_UUID, packet)
                print(f"Written config data to characteristic {CHARACTERISTIC_UUID}")                
            elif _cmd == 'r':
                # read config data
                header = struct.pack("<I B 3B", CHECK_CODE, 0x06, 0, 0, 0) # 0x06 is the command for reading config data
                await client.write_gatt_char(CHARACTERISTIC_UUID, header)
                print("Reading config data...")

                # Wait for a notification with the response
                response_data = await client.read_gatt_char(CHARACTERISTIC_UUID)
                if len(response_data) >= 8:
                    response_packet = struct.unpack("<I B 3B", response_data[:8])
                    check_code, cmd, parm1, parm2, parm3 = response_packet
                    if check_code == CHECK_CODE and cmd == 0x06:
                        config_data = response_data[8:]
                        ssid, password, device_number, dev_type = struct.unpack("<16s16sI B", config_data[:37])
                        print("Config Data:")
                        print("SSID:", ssid.decode().strip())
                        print("Password:", password.decode().strip())
                        print("Device Number:", device_number)
                        print("Device Type:", dev_type)
                    else:
                        print("Invalid response received")
                else:
                    print("Received data too short")
         # Make sure to stop the notification when done
        await client.stop_notify(CHARACTERISTIC_UUID)
        #disconnect
        await client.disconnect()

#%%
loop = asyncio.get_event_loop()
loop.run_until_complete(run(address, loop))
# %%
