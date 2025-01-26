
# import sys
# from PyQt6 import QtWidgets, uic
# from PyQt6.QtCore import QFile
# from PyQt6.QtCore import QFile, QThread, pyqtSignal

from PySide6.QtCore import QFile, QThread, Signal

import socket
import struct
import time

import math

import numpy as np

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
    connection_result = Signal(bool, str)
    response_received = Signal(int, int, bytes)
    daq_data_received = Signal(int, bytes)

    def __init__(self, ip, port):
        super().__init__()
        self.ip = ip
        self.port = port
        self.socket = None
        self.is_running = False

    def run(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(5)
            self.socket.connect((self.ip, self.port))
            self.connection_result.emit(True, "Connected successfully")
            
            # 스레드 실행 상태
            self.is_running = True
            
            while self.is_running:
                try:
                    # 헤더 수신
                    header = self.socket.recv(16)
                    if not header:
                        print("Connection closed by peer.")
                        break

                    magic, chip_id, packet_type, packet_size, _ = struct.unpack(
                        PACKET_HEADER_FORMAT, header
                    )
                    
                    if magic != MAGIC_NUMBER:
                        print("Invalid magic number")
                        continue

                    if packet_type == PacketType.DAQ:
                        # DAQ packet
                        daq_header = self.socket.recv(8)
                        sequence, data_size = struct.unpack(PACKET_DAQ_FORMAT, daq_header)
                        
                        data = bytearray()
                        while len(data) < data_size:
                            try:
                                packet = self.socket.recv(min(4096, data_size - len(data)))
                                if not packet:
                                    print("Connection closed during DAQ")
                                    break
                                data.extend(packet)
                            except socket.timeout:
                                print("Socket timeout, retrying DAQ...")
                                continue
                            except OSError as e:
                                # 소켓이 이미 종료된 경우 등
                                if not self.is_running:
                                    # stop() 요청으로 종료된 경우면 로그 없이 break
                                    break
                                else:
                                    print(f"Error receiving data: {str(e)}")
                                    break
                        
                        # 받은 데이터 emit
                        if len(data) > 0:
                            self.daq_data_received.emit(sequence, bytes(data))
                        
                    else:
                        # 일반(REQ/RES/SYS) 패킷 처리
                        if packet_size > 16:
                            body_size = packet_size - 16
                            body = self.socket.recv(body_size)
                            self.response_received.emit(packet_type, body_size, body)
                        else:
                            # packet_size가 16 이하라면 body 없음
                            self.response_received.emit(packet_type, 0, b"")
                            
                    print(f"Received packet: {packet_type}, {packet_size} , device : {chip_id}")
                        
                except socket.timeout:
                    # timeout이면 그냥 다시 loop
                    continue
                except OSError as e:
                    if not self.is_running:
                        # stop() 호출된 상태라면 조용히 종료
                        break
                    else:
                        print(f"Error receiving data: {str(e)}")
                        break
                except Exception as e:
                    print(f"Error receiving data: {str(e)}")
                    break
        
        except Exception as e:
            print(f"Error: {str(e)}")
            self.connection_result.emit(False, f"Connection failed: {str(e)}")

        finally:
            # 루프 종료 후 정리
            if self.socket:
                try:
                    self.socket.close()
                except:
                    pass
            self.socket = None

    def stop(self):
        """스레드 밖에서 호출되어, 소켓과 루프를 종료."""
        self.is_running = False

        # 이미 소켓이 열려 있다면 shutdown & close
        if self.socket:
            try:
                self.socket.shutdown(socket.SHUT_RDWR)
            except:
                pass
            try:
                self.socket.close()
            except:
                pass
            self.socket = None

    def send_packet(self, packet):
        """소켓이 살아있다면 패킷 전송."""
        if self.socket and self.is_running:
            try:
                self.socket.sendall(packet)
            except Exception as e:
                print(f"Error sending packet: {e}")
                
class GraphUpdateThread(QThread):
    update_signal = Signal(object)
    update_signal_bufferSize = Signal(int)
    
    maximum_extract_size = 800
    base_extract_size = 80

    def __init__(self, data_queues):
        """
        data_queues: [deque, deque, ..., deque]  # 8채널
        """
        super().__init__()
        self.data_queues = data_queues
        self.running = True

    def calculate_extract_size(self, queue_length):
        if queue_length <= 0:
            return self.base_extract_size
        log_factor = math.log(queue_length, 2)
        extract_size = int(self.base_extract_size * (1 + log_factor))
        extract_size = self.base_extract_size + int(extract_size/10)
        return min(extract_size, self.maximum_extract_size)

    def run(self):
        while self.running:
            time.sleep(0.01)  # ~100Hz

            # 각 채널별로 데이터가 얼마나 쌓였는지 확인
            queue_lengths = [len(dq) for dq in self.data_queues]
            min_len = min(queue_lengths)  # 가장 적은 길이를 기준으로 추출

            if min_len > 0:
                extract_size = self.calculate_extract_size(min_len)
                extract_size = min(extract_size, min_len)

                # 2차원 numpy array 생성: shape = (extract_size, 8)
                data_to_plot = np.zeros((extract_size, 8), dtype=np.float32)

                for i in range(extract_size):
                    for ch in range(8):
                        data_to_plot[i, ch] = self.data_queues[ch].popleft()

                # 시그널 emit: 여기서 data_to_plot은 2차원 np.ndarray
                self.update_signal.emit(data_to_plot)

                total_left = sum(len(dq) for dq in self.data_queues)
                self.update_signal_bufferSize.emit(total_left)

    def stop(self):
        self.running = False