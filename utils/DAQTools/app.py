import sys
from PyQt6 import QtWidgets, uic
from PyQt6.QtCore import QFile
from PyQt6.QtCore import QFile, QThread, pyqtSignal
import pyqtgraph as pg
from collections import deque

import socket
import struct
import numpy as np

from threadNets import ClientThread,PACKET_HEADER_FORMAT,PACKET_REQ_FORMAT,PACKET_RES_FORMAT,PacketType,Command,MAGIC_NUMBER

class MainWindow(QtWidgets.QMainWindow):
    
    def __init__(self):
        super().__init__()
        
        # UI 파일 로드
        ui_file = QFile("layout.ui")
        ui_file.open(QFile.OpenModeFlag.ReadOnly)
        uic.loadUi(ui_file, self)
        ui_file.close()
        
        self.connectionStatus = False
        
        # 그래프 위젯 생성
        self.graphWidget = pg.PlotWidget()
        layout = QtWidgets.QVBoxLayout(self.graphContainer)
        layout.addWidget(self.graphWidget)

        # 4개 채널에 대한 데이터 저장을 위한 배열 (각 채널당 500개 샘플)
        self.datas = np.zeros((4, 500), dtype=np.uint8)
        
        self.data = deque(maxlen=500)
        self.data.extend([0] * 500)  # 초기화

        # 4개의 선 그래프 생성
        self.curves = []
        colors = ['r', 'g', 'b', 'y']  # 각 채널별 색상
        # for i in range(4):
        #     curve = self.graphWidget.plot(pen=colors[i])
        #     self.curves.append(curve)
        
        # 1개의 선 그래프 생성 (채널 0만 표시)
        self.curve = self.graphWidget.plot(pen='b')  # 파란색 선으로 표시


        # 그래프 설정
        self.graphWidget.setTitle("Real-time 4-Channel Digital Data")
        self.graphWidget.setLabel('left', 'Value')
        self.graphWidget.setLabel('bottom', 'Sample')
        self.graphWidget.setYRange(0, 1)
        self.graphWidget.setXRange(0, 500)
        self.graphWidget.showGrid(x=True, y=True)

        # 버튼 연결
        self.btnConnect = self.findChild(QtWidgets.QPushButton, 'btnConnect')
        self.btnConnect.clicked.connect(self.connect_clicked)

        self.btnPing = self.findChild(QtWidgets.QPushButton, 'btnPing')
        self.btnPing.clicked.connect(self.ping_clicked)
        
        self.btnStartDAQ = self.findChild(QtWidgets.QPushButton, 'btn_startDAQ')
        self.btnStartDAQ.clicked.connect(self.startDAQ_clicked)
        
        self.client_thread = None
        self.response_callback = None
        
    def connect_clicked(self):
        if self.connectionStatus:
            self.disconnect()
        else:
            self.connect()
        
    def connect(self):
        self.btnConnect.setEnabled(False)
        self.statusBar().showMessage("Connecting...")
        
        ip = '192.168.4.82'
        port = 8284
        
        self.client_thread = ClientThread(ip, port)
        self.client_thread.connection_result.connect(self.on_connection_result)
        self.client_thread.response_received.connect(self.handle_response)
        self.client_thread.daq_data_received.connect(self.handle_daq_data)
        self.client_thread.start()
        
    def disconnect(self):
        if self.client_thread:
            self.client_thread.stop()
            self.client_thread = None
        self.connectionStatus = False
        self.btnConnect.setText("Connect")
        self.statusBar().showMessage("Disconnected")

    def on_connection_result(self, success, message):
        self.btnConnect.setEnabled(True)
        if success:
            self.connectionStatus = True
            self.btnConnect.setText("Disconnect")
        self.statusBar().showMessage(message)

    def handle_response(self, packet_type, packet_size, response):
        # if len(response) >= 16:
        #     resp_header = struct.unpack(PACKET_HEADER_FORMAT, response[:16])
            # if resp_header[0] == MAGIC_NUMBER:
                
        (rescode,p1,p2,p3) = struct.unpack(PACKET_RES_FORMAT, response)
                
        if packet_type == PacketType.SYS:
            # (rescode,) = struct.unpack(PACKET_RES_FORMAT, response[16:17])
            
            print(f"System message received: {rescode}")
        elif packet_type == PacketType.RES:
            
            check_code = p1
            print(f"Response received: {rescode}")
            if self.response_callback:
                self.response_callback(check_code, rescode)
                    # self.response_received.emit(rescode)
                    
                # 다른 패킷 타입에 대한 처리를 추가할 수 있습니다.
        else:
            print(f"Unknown packet type: {packet_type}")
            
    def handle_daq_data(self, sequence, data):
        
        # 데이터 처리
        # index = 0
        for byte in data:
            # 첫 번째 비트만 추출
            bit = byte & 0x01
            
            # self.data[index] = bit
            
            # index += 1
            # if index >= len(self.data):
            #     break
            
            # if(bit == 1):
                # print(f"Channel {index} is high")
            
            
            
            # # 데이터 배열 업데이트
            self.data = np.roll(self.data, -1)
            self.data[-1] = bit

        # 그래프 업데이트
        self.curve.setData(self.data)
            
            
        print(f"Received DAQ data. Sequence: {sequence}, Size: {len(data)}")
        
        

    def ping_clicked(self):
        if not self.connectionStatus:
            self.statusBar().showMessage("Not connected")
            return
        
        self.btnPing.setEnabled(False)

        header = struct.pack(PACKET_HEADER_FORMAT, MAGIC_NUMBER, 0, PacketType.REQ, 4, b'\x00' * 5)
        cmd = struct.pack(PACKET_REQ_FORMAT, Command.CMD_PING, 10,0,0)
        packet = header + cmd

        def _resCallback(check_code, rescode):
            self.btnPing.setEnabled(True)
            self.response_callback = None
            if check_code == 10:
                self.statusBar().showMessage("Ping success")
                print(f"Ping success: {rescode}")
            else:
                self.statusBar().showMessage(f"Ping failed: {rescode}")
                print(f"Ping failed: {rescode}")
                
        self.response_callback = _resCallback
        self.client_thread.send_packet(packet)
        
        #RES 패킷을 받으면 버튼 활성화 하기 위하여 콜백함수로 처리


    def startDAQ_clicked(self):
        print("Start DAQ button clicked")
        if not self.connectionStatus:
            self.statusBar().showMessage("Not connected")
            return

        try:
            # Start DAQ 패킷 생성
            header = struct.pack( PACKET_HEADER_FORMAT , MAGIC_NUMBER, 0, PacketType.REQ, 4, b'\x00' * 5)
            cmd = struct.pack( PACKET_REQ_FORMAT , Command.CMD_START_SAMPLING, 0,0,0)
            
            packet = header + cmd

            # 패킷 전송
            # self.socket.sendall(packet)
            self.client_thread.send_packet(packet)

        except Exception as e:
            print(f"Start DAQ failed: {str(e)}")
            self.statusBar().showMessage(f"Start DAQ failed: {str(e)}")
            self.disconnect()
        

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())