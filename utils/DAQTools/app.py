# app.py

import sys
# from PyQt6 import QtWidgets, uic
# from PyQt6.QtCore import QFile
# from PyQt6.QtCore import QFile, QThread, pyqtSignal

from PySide6 import QtWidgets

import pyqtgraph as pg
from collections import deque

# import socket
import os
import struct
import numpy as np

from threading import Thread, Event
from time import sleep, time

# threadNets.py 에 들어있는 클래스/함수들 불러오기
from threadNets import (
    ClientThread,
    PACKET_HEADER_FORMAT,
    PACKET_REQ_FORMAT,
    PACKET_RES_FORMAT,
    PacketType,
    Command,
    MAGIC_NUMBER,
    GraphUpdateThread,
)

from findDeviceDlg import FindDeviceDialog
from dataProcBasicForm import DataForm

# Qt Designer 등으로 만든 UI파일
import UI.mainWindow_ui as mainWindow_ui


class MainWindow(QtWidgets.QMainWindow, mainWindow_ui.Ui_MainWindow):
    def __init__(self):
        super().__init__()

        # UI 초기화
        self.setupUi(self)

        # 메뉴바 표시 설정
        self.menuBar().setVisible(True)

        # 상태 변수
        self.connectionStatus = False
        self.isCapturing = False  # 파일로 저장중인지 여부
        self.captureFile = None   # 파일 핸들

        # 버튼 연결
        self.btnConnect.clicked.connect(self.connect_clicked)
        self.btnPing.clicked.connect(self.onClickPing)
        self.btnPing.setEnabled(False)
        self.btn_startDAQ.clicked.connect(self.OnClickStartDAQ)
        self.btn_startDAQ.setEnabled(False)
        self.btnFindDevice.clicked.connect(self.OnClickFindDevice)
        self.btnCapture.clicked.connect(self.OnClickCapture)

        # 최근 접속한 주소 불러오기
        try:
            with open("recently_address.txt", "r") as f:
                ip = f.readline().strip()
                port = f.readline().strip()
                self.lineEdit_IP.setText(ip)
                self.lineEdit_Port.setText(port)
        except:
            pass

        # ClientThread / Callback
        self.client_thread = None
        self.response_callback = None

        # 8채널 데이터 보관용 deque
        self.datas = [deque(maxlen=40000) for _ in range(8)]

        # 그래프 위젯 생성
        self.setup_line_graph()

        # 그래프 업데이트 스레드 (8채널)
        self.update_thread = GraphUpdateThread(self.datas)
        self.update_thread.update_signal.connect(self.update_plot)
        self.update_thread.update_signal_bufferSize.connect(self.update_plot_dataInfo)
        self.update_thread.start()

        # 메뉴 액션 연결
        self.DataProcessBasicForm.triggered.connect(self.openDataProcessBasicForm)

    def openDataProcessBasicForm(self):
        print("Data Process Basic Form")
        self.dataForm = DataForm()
        self.dataForm.show()

    def setup_line_graph(self):
        """하나의 PlotWidget에 8채널 데이터를 그려주도록 구성."""
        self.graphWidget = pg.PlotWidget()
        layout = QtWidgets.QVBoxLayout(self.graphContainer)
        layout.addWidget(self.graphWidget)

        # 채널별 plot data 배열(rolling) 준비
        self.plot_len = 4000
        self.plot_datas = [
            np.zeros(self.plot_len, dtype=np.float32) for _ in range(8)
        ]

        # 채널별 Curve 생성
        self.curves = []
        colors = ["r", "g", "b", "c", "m", "y", "k", "w"]
        for ch in range(8):
            c = self.graphWidget.plot(self.plot_datas[ch], pen=colors[ch % len(colors)])
            self.curves.append(c)

        self.graphWidget.setTitle("Real-time 8-Channel Digital Data")
        self.graphWidget.setLabel("left", "Value")
        self.graphWidget.setLabel("bottom", "Sample")
        self.graphWidget.showGrid(x=True, y=True)

        # 8채널 디지털 신호를 구분하기 위해 Y범위를 넉넉히 잡습니다. (0/1 값이긴 하나, 채널별 offset 용도)
        self.graphWidget.setYRange(-1, 9)
        self.graphWidget.setXRange(0, self.plot_len)

        # X축만 줌 가능
        self.graphWidget.setMouseEnabled(x=True, y=False)

    def update_plot(self, new_data_2d):
        """
        GraphUpdateThread에서 emit된 8채널 2차원 데이터(new_data_2d)를 받아,
        rolling 버퍼에 추가 후 plot을 갱신.
        new_data_2d.shape = (N, 8)
        """
        if new_data_2d is None or len(new_data_2d) == 0:
            return

        n_new = len(new_data_2d)  # 새로 들어온 샘플 수

        # 8채널에 대해 각각 roll
        for ch in range(8):
            self.plot_datas[ch] = np.roll(self.plot_datas[ch], -n_new)
            self.plot_datas[ch][-n_new:] = new_data_2d[:, ch]

            # 0/1 파형을 단순히 그리면 겹치므로, 채널 ch에 일정 offset을 더해 줍니다.
            self.curves[ch].setData(self.plot_datas[ch] + ch)

    def update_plot_dataInfo(self, queSize):
        """큐(전체/합)에 남아있는 데이터 수를 표시."""
        self.labelInfo.setText(f"Received DAQ data. Buffer Size: {queSize}")

    def connect_clicked(self):
        if self.connectionStatus:
            self.disconnect()
        else:
            self.connect()

    def connect(self):
        try:
            self.btnConnect.setEnabled(False)
            self.statusBar().showMessage("Connecting...")

            ip = self.lineEdit_IP.text()
            port = int(self.lineEdit_Port.text())

            # IP/Port를 파일에 저장
            with open("recently_address.txt", "w") as f:
                f.write(f"{ip}\n{port}")

            self.client_thread = ClientThread(ip, port)
            self.client_thread.connection_result.connect(self.on_connection_result)
            self.client_thread.response_received.connect(self.handle_response)
            self.client_thread.daq_data_received.connect(self.handle_daq_data)
            self.client_thread.start()

        except Exception as e:
            print(f"Connection failed: {str(e)}")
            self.statusBar().showMessage(f"Connection failed: {str(e)}")
            self.btnConnect.setEnabled(True)

    def disconnect(self):
        if self.isCapturing:
            self.isCapturing = False
            self.captureFile.close()
            self.captureFile = None
            self.btnCapture.setText("Start Capture")

        self.btnConnect.setEnabled(False)

        # ClientThread 종료
        if self.client_thread:
            self.client_thread.stop()
            self.client_thread.wait()
            self.client_thread = None

        self.connectionStatus = False
        self.btnConnect.setText("Connect")
        self.statusBar().showMessage("Disconnected")
        self.btnConnect.setEnabled(True)
        self.btnPing.setEnabled(False)
        self.btn_startDAQ.setEnabled(False)

    def on_connection_result(self, success, message):
        self.btnConnect.setEnabled(True)
        sleep(1)
        if success:
            self.connectionStatus = True
            self.btnConnect.setText("Disconnect")

            # 연결되면 우선 DAQ Stop 신호 한 번 보냄
            self._sendStopDAQPacket()

            self.btnPing.setEnabled(True)
            self.btn_startDAQ.setEnabled(True)
            self.btn_startDAQ.setText("Start DAQ")

        self.statusBar().showMessage(message)

    def handle_response(self, packet_type, packet_size, response):
        (rescode, p1, p2, p3) = struct.unpack(PACKET_RES_FORMAT, response)

        if packet_type == PacketType.SYS:
            print(f"System message received: {rescode}")
        elif packet_type == PacketType.RES:
            check_code = p1
            print(f"Response received: {rescode}")
            if self.response_callback:
                self.response_callback(check_code, rescode)
        else:
            print(f"Unknown packet type: {packet_type}")

    def handle_daq_data(self, sequence, data: bytes):
        """
        DAQ 데이터(여러 바이트)를 받으면,
        8채널(비트 분리)하여 datas[ch]에 저장.
        """
        for byte in data:
            for ch in range(8):
                bit = (byte >> ch) & 0x01
                self.datas[ch].append(bit)

        if self.isCapturing:
            self.captureFile.write(data)

    def onClickPing(self):
        if not self.connectionStatus:
            self.statusBar().showMessage("Not connected")
            return

        self.btnPing.setEnabled(False)

        header = struct.pack(
            PACKET_HEADER_FORMAT,
            MAGIC_NUMBER,
            0,
            PacketType.REQ,
            4,
            b"\x00" * 5,
        )
        cmd = struct.pack(PACKET_REQ_FORMAT, Command.CMD_PING, 10, 0, 0)
        packet = header + cmd

        def _resCallback(check_code, rescode):
            self.btnPing.setEnabled(True)
            self.response_callback = None
            if check_code == 10:
                self.statusBar().showMessage("Ping success")
                print(f"Ping success: {rescode}")
                ping_timeout_event.set()
            else:
                self.statusBar().showMessage(f"Ping failed: {rescode}")
                print(f"Ping failed: {check_code}")

        def waitThread():
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

    def _sendStopDAQPacket(self):
        """Stop DAQ 패킷 생성 후 전송."""
        header = struct.pack(
            PACKET_HEADER_FORMAT,
            MAGIC_NUMBER,
            0,
            PacketType.REQ,
            4,
            b"\x00" * 5,
        )
        cmd = struct.pack(PACKET_REQ_FORMAT, Command.CMD_STOP_SAMPLING, 0, 0, 0)
        packet = header + cmd
        self.client_thread.send_packet(packet)

    def OnClickStartDAQ(self):
        if not self.connectionStatus:
            self.statusBar().showMessage("Not connected")
            return

        try:
            if self.btn_startDAQ.text() == "Stop DAQ":
                # Stop DAQ
                self._sendStopDAQPacket()
                self.btn_startDAQ.setText("Start DAQ")
                self.statusBar().showMessage("DAQ stopped")
            else:
                # Start DAQ
                self.stopDAQ = True
                bufferSize = int(self.leBufferLimt.text())

                header = struct.pack(
                    PACKET_HEADER_FORMAT,
                    MAGIC_NUMBER,
                    0,
                    PacketType.REQ,
                    4,
                    b"\x00" * 5,
                )
                cmd = struct.pack(
                    PACKET_REQ_FORMAT,
                    Command.CMD_START_SAMPLING,
                    (bufferSize >> 8) & 0xFF,
                    bufferSize & 0xFF,
                    0,
                )
                packet = header + cmd
                self.client_thread.send_packet(packet)

                self.btn_startDAQ.setText("Stop DAQ")
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
                return chipid, address
            else:
                print("No device selected")
        else:
            print("Cancel clicked")

        dialog.close()
        return None, None

    def get_next_capture_filename(self):
        index = 1
        while True:
            filename = f"capture_{index:03d}.bin"
            if not os.path.exists(filename):
                return filename
            index += 1

    def OnClickCapture(self):
        try:
            print("Capture button clicked")
            if not self.isCapturing:
                filename = self.get_next_capture_filename()
                self.captureFile = open(filename, "ab")
                self.isCapturing = True
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

    def closeEvent(self, event):
        """창을 닫을 때 정리 작업."""
        if self.isCapturing:
            self.isCapturing = False
            self.captureFile.close()
            self.captureFile = None
            print("Capture stopped")

        if self.client_thread:
            self.client_thread.stop()
            self.client_thread = None

        if self.update_thread:
            self.update_thread.stop()
            self.update_thread.wait()
            print("Update thread stopped")

        super().closeEvent(event)
        print("Close event completed")


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
