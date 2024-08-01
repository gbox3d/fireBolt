
import sys
from PyQt6 import QtWidgets, uic
from PyQt6.QtCore import QFile
from PyQt6.QtCore import QFile, QThread, pyqtSignal

import socket
import struct

MAGIC_NUMBER = 20240729

# 패킷 타입
class PacketType:
    REQ = 0
    RES = 1
    SYS = 2
    DAQ = 3

# 명령어
class Command:
    CMD_PING = 0
    CMD_START_SAMPLING = 1
    CMD_STOP_SAMPLING = 2
    CMD_PUT_SAMPLING_DATA = 3
    
# 패킷 구조체 정의
PACKET_HEADER_FORMAT = "<LLBH5s"  # !: 네트워크 바이트 순서, I: unsigned int, B: unsigned char, H: unsigned short, 5s: 5바이트 문자열
PACKET_REQ_FORMAT =  "<BBBB"  # B: cmd, 3s: param
PACKET_RES_FORMAT =  "<BBBB"  # B: res code
PACKET_DAQ_FORMAT = "<LL"  # L: sequence, L: data size


class ClientThread(QThread):
    connection_result = pyqtSignal(bool, str)
    response_received = pyqtSignal(int,int,bytes)
    daq_data_received = pyqtSignal(int,bytes)

    def __init__(self, ip, port):
        super().__init__()
        self.ip = ip
        self.port = port
        self.socket = None
        self.is_running = False
        
    def receive_exact(sock, size):
        data = bytearray()
        print(f"Receiving {size} bytes")
        while len(data) < size:
            packet = sock.recv(size - len(data))
            if not packet:
                print("Connection closed")
                break;
            data.extend(packet)
            print(f"Received {len(packet)} bytes")
        return data

    def run(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(5)
            self.socket.connect((self.ip, self.port))
            self.connection_result.emit(True, "Connected successfully")
            
            self.is_running = True
            
            while self.is_running:
                header = self.socket.recv(16)
                if not header or len(header) != 16:
                    break

                magic, chip_id, packet_type, packet_size, _ = struct.unpack(PACKET_HEADER_FORMAT, header)
                
                if magic != MAGIC_NUMBER:
                    print("Invalid magic number")
                    continue

                if packet_type == PacketType.DAQ:
                    print("DAQ packet received")
                    daq_header = self.socket.recv(8)
                    sequence, data_size = struct.unpack(PACKET_DAQ_FORMAT, daq_header)
                    
                    
                    
                    print(f"Sequence: {sequence}, Data size: {data_size}")
                    
                    data = bytearray()
                    while len(data) < data_size:
                        try:
                            packet = self.socket.recv(min(4096, data_size - len(data)))
                            if not packet:
                                print("Connection closed")
                                break
                            data.extend(packet)
                            print(f"Received {len(packet)} bytes, Total: {len(data)}/{data_size}")
                        except socket.timeout:
                            print("Socket timeout, retrying...")
                            continue
                        except Exception as e:
                            print(f"Error receiving data: {str(e)}")
                            break
                    print(f"Data received: {len(data)}")
                    
                    
                    
                    self.daq_data_received.emit(sequence, bytes(data))
                    
                else:
                    body = self.socket.recv(packet_size - 16)
                    self.response_received.emit(packet_type, packet_size - 16,body)
            
            
        except Exception as e:
            print(f"Error: {str(e)}")
            self.connection_result.emit(False, f"Connection failed: {str(e)}")

    def stop(self):
        self.is_running = False
        if self.socket:
            self.socket.close()

    def send_packet(self, packet):
        if self.socket:
            self.socket.sendall(packet)