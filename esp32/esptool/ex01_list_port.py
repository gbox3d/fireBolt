#%%
import esptool
import serial.tools.list_ports
import sys

#%%
def detect_chip_on_port(port_name):
    """주어진 포트에서 ESP32 칩을 감지합니다."""
    try:
        # 시리얼 연결 설정, 타임아웃 설정 포함
        ser = serial.Serial(port_name, baudrate=115200, timeout=1)
        esp = esptool.ESPLoader(port=ser)
        esp.connect()
        chip_info = esp.get_chip_description()
        print(f"Detected {chip_info} on {port_name}")
        ser.close()
    except (esptool.FatalError, serial.SerialException) as e:
        print(f"Failed to detect ESP32 on {port_name}, {e}")
    except serial.SerialTimeoutException as e:
        print(f"SerialTimeoutException on {port_name}, {e}")
    except Exception as e:
        print(f"Unexpected error on {port_name}, {e}")
#%%
ports = serial.tools.list_ports.comports()
# available_ports = []
for port_name, desc, hwid in sorted(ports):
    print(f"{port_name}: {desc} [{hwid}]")
    # detect_chip_on_port(port_name)
    
        



# %%
