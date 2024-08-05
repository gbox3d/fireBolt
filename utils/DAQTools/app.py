import sys
from PyQt6 import QtWidgets, uic
from PyQt6.QtCore import QFile
from PyQt6.QtCore import QFile, QThread, pyqtSignal
import pyqtgraph as pg
from collections import deque


# import socket
import os
import struct
import numpy as np

from time import sleep, time

from threadNets import ClientThread,PACKET_HEADER_FORMAT,PACKET_REQ_FORMAT,PACKET_RES_FORMAT,PacketType,Command,MAGIC_NUMBER 
from threadNets import GraphUpdateThread

from findDeviceDlg import FindDeviceDialog

from threading import Thread,Event

        
        
class MainWindow(QtWidgets.QMainWindow):
    
    def __init__(self):
        super().__init__()
        
        # UI 파일 로드
        ui_file = QFile("layout.ui")
        ui_file.open(QFile.OpenModeFlag.ReadOnly)
        uic.loadUi(ui_file, self)
        ui_file.close()
        
         # 메뉴바 표시 설정
        self.menuBar().setVisible(True)
        
        self.connectionStatus = False
        self.isCapturing = False # 파일로 저장중인지 여부
        self.captureFile = None  # 파일 핸들
        
        # 그래프 위젯 생성
        self.setup_line_graph()

        # 버튼 연결
        self.btnConnect.clicked.connect(self.connect_clicked)

        # self.btnPing = self.findChild(QtWidgets.QPushButton, 'btnPing')
        self.btnPing.clicked.connect(self.onClickPing)
        
        # self.btnStartDAQ = self.findChild(QtWidgets.QPushButton, 'btn_startDAQ')
        self.btn_startDAQ.clicked.connect(self.OnClickStartDAQ)
        
        # self.btnFindDevice = self.findChild(QtWidgets.QPushButton, 'btnFindDevice')
        self.btnFindDevice.clicked.connect(self.OnClickFindDevice)
        
        self.btnCapture.clicked.connect(self.OnClickCapture)
        
        
        # IP, Port 입력 필드
        self.leIpAddress = self.findChild(QtWidgets.QLineEdit, 'lineEdit_IP')
        self.lePort = self.findChild(QtWidgets.QLineEdit, 'lineEdit_Port')
        
        # 최근 접속한 주소 불러오기
        try:
            with open("recently_address.txt", "r") as f:
                ip = f.readline().strip()
                port = f.readline().strip()
                self.leIpAddress.setText(ip)
                self.lePort.setText(port)
        except:
            pass
        
        self.client_thread = None
        self.response_callback = None
        # self.ping_timeout_event = Event()
        
        # 그래프 업데이트 스레드 시작
        self.update_thread = GraphUpdateThread(self.data)
        self.update_thread.update_signal.connect(self.update_plot)
        self.update_thread.update_signal_bufferSize.connect(self.update_plot_dataInfo)
        self.update_thread.start()
        
        
        
    def setup_line_graph(self):
        self.graphWidget = pg.PlotWidget()
        layout = QtWidgets.QVBoxLayout(self.graphContainer)
        layout.addWidget(self.graphWidget)

        # 4개 채널에 대한 데이터 저장을 위한 배열 (각 채널당 500개 샘플)
        # self.datas = np.zeros((4, 500), dtype=np.uint8)
        
        self.data = deque(maxlen=40000)
        # self.data.extend([0] * 10000)  # 초기화
        # 그래프 생성
        plot_len = 4000
        self.curves = []
        # 그래프에 표시할 데이터
        self.plot_data = np.zeros(plot_len)
        self.curve = self.graphWidget.plot(self.plot_data, pen='g')  # 파란색 선으로 표시
        # self.curve = self.graphWidget.plot(pen='g')  # 파란색 선으로 표시

        # 그래프 설정
        self.graphWidget.setTitle("Real-time 4-Channel Digital Data")
        self.graphWidget.setLabel('left', 'Value')
        self.graphWidget.setLabel('bottom', 'Sample')
        self.graphWidget.setYRange(0, 1)
        self.graphWidget.setXRange(0, plot_len)
        self.graphWidget.showGrid(x=True, y=True)
        
        # X축에 대해서만 확대/축소 허용
        self.graphWidget.setMouseEnabled(x=True, y=False)
        
    def update_plot(self, new_data):
        self.plot_data = np.roll(self.plot_data, -len(new_data))
        self.plot_data[-len(new_data):] = new_data
        self.curve.setData(self.plot_data)
    def update_plot_dataInfo(self, queSize):
        self.labelInfo.setText(f"Received DAQ data. Buffer Size: {queSize}")

        
    def connect_clicked(self):
        if self.connectionStatus:
            self.disconnect()
        else:
            self.connect()
        
    def connect(self):
        self.btnConnect.setEnabled(False)
        self.statusBar().showMessage("Connecting...")
        
        # ip = '192.168.4.82'
        # port = 8284
        
        ip = self.leIpAddress.text()
        port = int(self.lePort.text())
        
        # recently address 파일에 저장 , 나중에 로드할때 사용
        with open("recently_address.txt", "w") as f:
            f.write(f"{ip}\n{port}")
        
        self.client_thread = ClientThread(ip, port)
        self.client_thread.connection_result.connect(self.on_connection_result)
        self.client_thread.response_received.connect(self.handle_response)
        self.client_thread.daq_data_received.connect(self.handle_daq_data)
        self.client_thread.start()
        
    def disconnect(self):
        
        self.btnConnect.setEnabled(False)
        
        event_timeout = Event()
        
        def _wait_disconnect():
            if self.client_thread:
                self.client_thread.stop()
                self.client_thread = None
            self.connectionStatus = False
            self.btnConnect.setText("Connect")
            self.statusBar().showMessage("Disconnected")
            self.btnConnect.setEnabled(True)
            event_timeout.set() # Signal to stop the timeout thread
            
        wait_thread = Thread(target=_wait_disconnect)
        wait_thread.start()
        
        # 10초 이내에 연결이 끊기지 않으면 타임아웃 메시지 출력
        def _timeout():
            timeout_occurred = not event_timeout.wait(10)
            if timeout_occurred:
                self.btnConnect.setEnabled(True)
                self.statusBar().showMessage("Disconnect timeout")
                print("Disconnect timeout")
        
        timeout_thread = Thread(target=_timeout)
        timeout_thread.start()

    def on_connection_result(self, success, message):
        self.btnConnect.setEnabled(True)
        if success:
            self.connectionStatus = True
            self.btnConnect.setText("Disconnect")
        self.statusBar().showMessage(message)

    def handle_response(self, packet_type, packet_size, response):
        
        (rescode,p1,p2,p3) = struct.unpack(PACKET_RES_FORMAT, response)
        
        if packet_type == PacketType.SYS:
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
        for byte in data:
            # 첫 번째 비트만 추출
            bit = byte & 0x01
            self.data.append(bit)
            
        if self.isCapturing:
            self.captureFile.write(data)
        
    def onClickPing(self):
        if not self.connectionStatus:
            self.statusBar().showMessage("Not connected")
            return
        
        self.btnPing.setEnabled(False)

        header = struct.pack(PACKET_HEADER_FORMAT, MAGIC_NUMBER, 0, PacketType.REQ, 4, b'\x00' * 5)
        cmd = struct.pack(PACKET_REQ_FORMAT, Command.CMD_PING, 10,0,0)
        packet = header + cmd
        
        # curTick = int(.time())

        def _resCallback(check_code, rescode):
            self.btnPing.setEnabled(True)
            self.response_callback = None
            if check_code == 10:
                self.statusBar().showMessage("Ping success")
                print(f"Ping success: {rescode}")
                ping_timeout_event.set()  # Signal to stop the wait threa
            else:
                self.statusBar().showMessage(f"Ping failed: {rescode}")
                print(f"Ping failed: {rescode}")
        
        # def waitThred():
        #     sleep(5)
        #     self.btnPing.setEnabled(True)
        #     self.response_callback = None
        #     self.statusBar().showMessage("Ping failed")
        #     print(f"Ping failed")
        
        def waitThread():
            # Wait for 5 seconds or until the event is set
            timeout_occurred = not ping_timeout_event.wait(5)
            if timeout_occurred:
                self.btnPing.setEnabled(True)
                self.response_callback = None
                self.statusBar().showMessage("Ping failed")
                
        ping_timeout_event = Event()
        wait_thread = Thread(target=waitThread)
        wait_thread.start()
                
        self.response_callback = _resCallback
        self.client_thread.send_packet(packet)
        
        #RES 패킷을 받으면 버튼 활성화 하기 위하여 콜백함수로 처리

    def OnClickStartDAQ(self):
        print("Start DAQ button clicked")
        if not self.connectionStatus:
            self.statusBar().showMessage("Not connected")
            return

        try:
            if self.btn_startDAQ.text() == "Stop DAQ":
                # Stop DAQ 패킷 생성
                header = struct.pack( PACKET_HEADER_FORMAT , MAGIC_NUMBER, 0, PacketType.REQ, 4, b'\x00' * 5)
                cmd = struct.pack( PACKET_REQ_FORMAT , Command.CMD_STOP_SAMPLING, 0,0,0)
                
                packet = header + cmd

                # 패킷 전송
                # self.socket.sendall(packet)
                self.client_thread.send_packet(packet)
                self.btn_startDAQ.setText("Start DAQ")
                self.statusBar().showMessage("DAQ stopped")
                # self.stopDAQ = False
                # return
            else :
                
                
                
                #  Stop DAQ 패킷 생성
            
                self.stopDAQ = True
                
                header = struct.pack( PACKET_HEADER_FORMAT , MAGIC_NUMBER, 0, PacketType.REQ, 4, b'\x00' * 5)
                cmd = struct.pack( PACKET_REQ_FORMAT , Command.CMD_START_SAMPLING, 0,0,0)
                
                packet = header + cmd

                # 패킷 전송
                # self.socket.sendall(packet)
                self.client_thread.send_packet(packet)
                
                # self.btn_startDAQ.setEnabled(False)
                # sleep(5)
                
                self.btn_startDAQ.setText("Stop DAQ")
                # self.btn_startDAQ.setEnabled(True)
                self.statusBar().showMessage("DAQ Stop")
                
        except Exception as e:
            print(f"Start DAQ failed: {str(e)}")
            self.statusBar().showMessage(f"Start DAQ failed: {str(e)}")
            self.disconnect()
            
    def OnClickFindDevice(self): 
        print("Find Device button clicked")
        dialog = FindDeviceDialog(self)
        result = dialog.exec()
        
        if result == QtWidgets.QDialog.DialogCode.Accepted:
            print("OK clicked")
            chipid, address = dialog.get_selected_address()
            if address:
                print(f"Selected device chipid: {chipid}")
                print(f"Selected device address: {address}")
                
                self.lineEdit_IP.setText(address)
                
                return chipid, address  # 선택된 chipid와 주소 반환
            else:
                print("No device selected")
        else:
            print("Cancel clicked")
            
        dialog.close()
        
        return None, None  # 취소되거나 선택된 장치가 없는 경우
    
    def get_next_capture_filename(self):
        index = 1
        while True:
            filename = f"capture_{index:03d}.bin"
            if not os.path.exists(filename):
                return filename
            index += 1
            
    def OnClickCapture(self):
        try :
            print("Capture button clicked")
            if self.isCapturing == False:
                filename = self.get_next_capture_filename()
                self.captureFile = open(filename, "ab")
                self.isCapturing = True
                # self.captureFile = open("capture.bin", "wb")
                self.btnCapture.setText("Stop Capture")
                self.statusBar().showMessage("Capture started")
            else:
                self.isCapturing = False
                self.captureFile.close()
                self.captureFile = None
                self.btnCapture.setText("Start Capture")
                self.statusBar().showMessage("Capture stopped")
        except Exception as e:
            print(f"Capture failed: {str(e)}")
            self.statusBar().showMessage(f"Capture failed: {str(e)}")
            # self.disconnect()
        
if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())