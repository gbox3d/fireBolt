# serial_handler.py
import serial
# import serial.tools.list_ports
from serial.tools import list_ports  # pyserial 패키지의 list_ports 모듈을 임포트합니다.
from PySide6.QtCore import QThread, Signal

# import wmi


class SerialThread(QThread):
    data_received = Signal(str)

    def __init__(self, port_name, baudrate=9600):
        super().__init__()
        self.serial_port = serial.Serial(port_name, baudrate=baudrate, timeout=1)
        self.running = True

    def run(self):
        """Thread run method. Continuously read from serial port."""
        while self.running:
            if self.serial_port.in_waiting > 0:
                try:
                    data = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
                    self.data_received.emit(data)
                except UnicodeDecodeError as e:
                    print(f"Decoding error: {e}")
                    continue

    def stop(self):
        """Stops the thread and closes the serial port."""
        self.running = False
        if self.serial_port.is_open:
            self.serial_port.close()

# def get_bluetooth_device_names():
#     """Get paired Bluetooth device names and their associated COM ports using WMI."""
#     bt_devices = {}
#     wmi_obj = wmi.WMI()
    
#     # Query WMI for Bluetooth devices associated with COM ports
#     bt_com_ports = wmi_obj.query("SELECT DeviceID, Name FROM Win32_PnPEntity WHERE Name LIKE '%(COM%' AND Description LIKE '%Bluetooth%'")
    
#     for device in bt_com_ports:
#         # Extract the COM port number and Bluetooth device name
#         device_id = device.DeviceID
#         device_name = device.Name
#         bt_devices[device_id] = device_name
    
#     return bt_devices


def scan_serial_ports():
    """Scan for available serial ports and combine them with paired Bluetooth names."""
    ports = list_ports.comports()
    # bt_devices = get_bluetooth_device_names()  # Get Bluetooth device names via WMI
    
    port_list = []
    for port in ports:
        port_list.append({
            "device": port.device,
            "description": port.description,
            "hwid": port.hwid
        })
        # port_dict[port.device] = {
        #     "port": port.device,
        #     "description": port.description,
        #     "hwid": port.hwid
        # }

    return port_list


def open_serial_port(port_name, baudrate=9600):
    """Open a serial port."""
    try:
        serial_port = serial.Serial(port_name, baudrate=baudrate, timeout=1)
        return serial_port
    except serial.SerialException as e:
        raise e
