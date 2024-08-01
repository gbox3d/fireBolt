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

# 명령어
class Command:
    CMD_PING = 0
    CMD_START_SAMPLING = 1
    CMD_STOP_SAMPLING = 2
    
# 패킷 구조체 정의
PACKET_HEADER_FORMAT = "<LLBH5s"  # !: 네트워크 바이트 순서, I: unsigned int, B: unsigned char, H: unsigned short, 5s: 5바이트 문자열
PACKET_REQ_FORMAT =  "<B3s"  # B: cmd, 3s: param
PACKET_RES_FORMAT =  "<B"  # B: res code



class ConnectThread(QThread):
    connection_result = pyqtSignal(bool, str, socket.socket)

    def __init__(self, ip, port):
        super().__init__()
        self.ip = ip
        self.port = port

    def run(self):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)  # 5초 타임아웃 설정
            sock.connect((self.ip, self.port))
            self.connection_result.emit(True, "Connected successfully", sock)
        except Exception as e:
            self.connection_result.emit(False, f"Connection failed: {str(e)}", None)

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        
        # UI 파일 로드
        ui_file = QFile("layout.ui")
        ui_file.open(QFile.OpenModeFlag.ReadOnly)
        uic.loadUi(ui_file, self)
        ui_file.close()
        
        self.connectionStatus = False

        # 버튼 연결
        self.btnConnect = self.findChild(QtWidgets.QPushButton, 'btnConnect')
        self.btnConnect.clicked.connect(self.connect_clicked)

        self.btnPing = self.findChild(QtWidgets.QPushButton, 'btnPing')
        self.btnPing.clicked.connect(self.ping_clicked)
        
    def connect(self):
        self.btnConnect.setEnabled(False)
        self.statusBar().showMessage("Connecting...")
        
        ip = '192.168.4.82'
        port = 8284
        
        self.connect_thread = ConnectThread(ip, port)
        self.connect_thread.connection_result.connect(self.on_connection_result)
        self.connect_thread.start()

    def disconnect(self):
        if self.socket:
            self.socket.close()
            self.socket = None
        self.connectionStatus = False
        self.btnConnect.setText("Connect")
        self.statusBar().showMessage("Disconnected")

    def connect_clicked(self):
        if self.connectionStatus:
            self.disconnect()
        else:
            self.connect()
            
    def on_connection_result(self, success, message,sock):
        self.btnConnect.setEnabled(True)
        if success:
            self.connectionStatus = True
            self.socket = sock
            self.btnConnect.setText("Disconnect")
            self.statusBar().showMessage(message)
        else:
            self.statusBar().showMessage(message)
            QtWidgets.QMessageBox.critical(self, "Connection Error", message)


        

    def ping_clicked(self):
        print("Ping button clicked")
        if not self.connectionStatus:
            self.statusBar().showMessage("Not connected")
            return

        try:
            # Ping 패킷 생성
            header = struct.pack( PACKET_HEADER_FORMAT , MAGIC_NUMBER, 0, PacketType.REQ, 4, b'\x00' * 5)
            cmd = struct.pack( PACKET_REQ_FORMAT , Command.CMD_PING, b'\x00' * 3)
            
            packet = header + cmd

            # 패킷 전송
            self.socket.sendall(packet)

            # 응답 대기
            response = self.socket.recv(1024)
            if len(response) >= 20:  # 헤더 크기
                # resp_header = struct.unpack(, response[:20])
                resp_header = struct.unpack(PACKET_HEADER_FORMAT, response[:16])
                if resp_header[0] == MAGIC_NUMBER and resp_header[2] == PacketType.RES:
                    (rescode,) = struct.unpack(PACKET_RES_FORMAT, response[16:17])
                    print(f"Ping successful: {rescode}")
                    self.statusBar().showMessage("Ping successful")
                else:
                    self.statusBar().showMessage("Invalid response")
            else:
                print(f"response length too short : {len(response)}")
                self.statusBar().showMessage("No response")

        except Exception as e:
            print(f"Ping failed: {str(e)}")
            self.statusBar().showMessage(f"Ping failed: {str(e)}")
            self.disconnect()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())