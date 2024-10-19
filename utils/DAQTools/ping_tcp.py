#%%
import socket
import time
import struct

from threadNets import ClientThread,PACKET_HEADER_FORMAT,PACKET_REQ_FORMAT,PACKET_RES_FORMAT,PacketType,Command,MAGIC_NUMBER 

#%%
_IP = "192.168.0.12"
_PORT = 8284
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcp_socket.settimeout(5)
tcp_socket.connect((_IP, _PORT))

print("connected")

# %%
header = struct.pack(PACKET_HEADER_FORMAT, MAGIC_NUMBER, 0, PacketType.REQ, 20, b'\x00' * 5)
cmd = struct.pack(PACKET_REQ_FORMAT, Command.CMD_PING, 10,0,0)
packet = header + cmd

tcp_socket.sendall(packet)

data = tcp_socket.recv(20)
header = struct.unpack(PACKET_HEADER_FORMAT, data[:16])
print(header)
result_code,checkcode,p2,p3 = struct.unpack(PACKET_RES_FORMAT, data[16:20])

print(f"result_code: {result_code}, checkcode: {checkcode}, p2: {p2}, p3: {p3}")

# %%

tcp_socket.close()
print("disconnected")

# %%
