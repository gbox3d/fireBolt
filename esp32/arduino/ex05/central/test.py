#%%
import sys
import asyncio
from bleak import BleakClient

# Your device's MAC address (replace with actual address)
address = "B90608F9-6C0D-3D90-561F-E2D2909A50FA"
# The UUIDs of the service and characteristic
SERVICE_UUID = "02cb14bf-42e0-41bd-9d6d-68db02057e42"
CHARACTERISTIC_UUID = "fef6efb5-fbf8-4d0a-a69f-57049733a87a"

async def run(address, loop):
    async with BleakClient(address, loop=loop) as client:
        # Reading the welcome message from the characteristic
        welcome_message = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(f"Received welcome message: {welcome_message.decode('utf-8')}")
        # Adding a delay to ensure the ESP32 has processed the message
        await asyncio.sleep(1) # 1-second delay

        # Writing a string value to the characteristic
        value = bytearray("Hello, ESP32!", 'utf-8')
        await client.write_gatt_char(CHARACTERISTIC_UUID, value)
        print(f"Written value {value} to characteristic {CHARACTERISTIC_UUID}")
        
        # Adding a delay to ensure the ESP32 has processed the message
        await asyncio.sleep(1) # 1-second delay

        # Reading the echoed value from the characteristic
        response = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(f"Read echoed value {response.decode('utf-8')} from characteristic {CHARACTERISTIC_UUID}")
#%%
loop = asyncio.get_event_loop()
loop.run_until_complete(run(address, loop))

# %%
