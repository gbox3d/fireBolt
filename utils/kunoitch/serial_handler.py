# serial_handler.py
import serial
from serial.tools import list_ports  # pyserial 패키지의 list_ports 모듈을 임포트합니다.
from PySide6.QtCore import QThread, Signal


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


def scan_serial_ports():
    """Scan for available serial ports."""
    return list_ports.comports()


def open_serial_port(port_name, baudrate=9600):
    """Open a serial port."""
    try:
        serial_port = serial.Serial(port_name, baudrate=baudrate, timeout=1)
        return serial_port
    except serial.SerialException as e:
        raise e
