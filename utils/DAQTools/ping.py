#%%
import socket
import time
from struct import *

# UDP 소켓 생성
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_socket.settimeout(1)  # 1초 타임아웃 설정

# ESP32의 IP 주소와 포트
ESP32_IP = "192.168.4.81"
ESP32_PORT = 8284

# 매직 넘버
MAGIC_NUMBER = 20240729

# 패킷 타입
class PacketType:
    REQ = 0
    RES = 1
    SYS = 2

# 명령어
class Command:
    CMD_PING = 0
    CMD_START_SAMPLING = 1
    CMD_STOP_SAMPLING = 2

# 패킷 구조체 정의
PACKET_HEADER_FORMAT = "<LLBH5s"  # !: 네트워크 바이트 순서, I: unsigned int, B: unsigned char, H: unsigned short, 5s: 5바이트 문자열
PACKET_REQ_FORMAT = PACKET_HEADER_FORMAT + "B3s"  # B: cmd, 3s: param

def create_ping_packet():
    header = pack(PACKET_HEADER_FORMAT,
                  MAGIC_NUMBER,  # header
                  0,             # chipId (클라이언트에서는 0으로 설정)
                  PacketType.REQ,  # type (0: REQ)
                  4,             
                  b'\x00' * 5)   # extra 배열
    
    cmd = Command.CMD_PING
    param = b'\x00' * 3
    
    return pack(PACKET_REQ_FORMAT, *unpack(PACKET_HEADER_FORMAT, header), cmd, param)

def ping_test(num_pings=5):
    for i in range(num_pings):
        packet = create_ping_packet()
        
        start_time = time.time()
        
        try:
            # 패킷 전송
            udp_socket.sendto(packet, (ESP32_IP, ESP32_PORT))
            
            # 응답 대기
            data, addr = udp_socket.recvfrom(1024)
            
            end_time = time.time()
            rtt = (end_time - start_time) * 1000  # RTT를 밀리초 단위로 계산
            
            # 응답 패킷 분석
            received_header = unpack(PACKET_HEADER_FORMAT, data[:16])
            if received_header[0] == MAGIC_NUMBER and received_header[2] == PacketType.RES:
                result_code = unpack("!B", data[16:17])[0]
                print(f"Ping {i+1}: RTT = {rtt:.2f} ms, Result Code = {result_code}")
            else:
                print(f"Ping {i+1}: Invalid response received")
        
        except socket.timeout:
            print(f"Ping {i+1}: Request timed out")
        
        time.sleep(1)  # 1초 대기

#%%
ping_test(5)
# %%
