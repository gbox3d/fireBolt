import sys
from PyQt6 import QtWidgets, uic
from PyQt6.QtCore import QFile
from PyQt6.QtCore import QFile, QThread, pyqtSignal
import pyqtgraph as pg
from collections import deque

# data_form.py

from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QFileDialog

from procData import process_channel_data, export_to_csv, read_and_process_data

import numpy as np


class DataForm(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        
        # UI 파일 로드
        uic.loadUi('dataProcBasic.ui', self)
        
        # 버튼 연결
        self.btnLoadData.clicked.connect(self.load_data)
        
        # 위젯 참조
        self.labelInfo = self.findChild(QtWidgets.QLabel, 'labelInfo')
        
        # PyQtGraph 위젯 설정
        self.setup_line_graph()
        
    def setup_line_graph(self):
        # PyQtGraph 위젯 생성
        self.plot_widget = pg.PlotWidget()
        
        # widgetLineGraphContainer에 그래프 추가
        layout = QtWidgets.QVBoxLayout(self.widgetLineGraphContainer)
        layout.addWidget(self.plot_widget)
        
        # 그래프 스타일 설정
        self.plot_widget.setBackground('w')  # 흰색 배경
        self.plot_widget.showGrid(x=True, y=True)
        self.plot_widget.setLabel('left', 'Value')
        self.plot_widget.setLabel('bottom', 'Index')
        
        #y축 범위 설정
        self.plot_widget.setYRange(0, 1.1,padding=0)
        self.plot_widget.setXRange(0, 1000)
        
        # X축에 대해서만 확대/축소 허용
        self.plot_widget.setMouseEnabled(x=True, y=False)

        # 마우스 드래그로 X축만 이동 가능하게 설정
        # self.plot_widget.getPlotItem().getViewBox().setMouseMode(pg.ViewBox.RectMode)
        
    def load_data(self):
        file_name, _ = QFileDialog.getOpenFileName(self, "Open File", "", "All Files (*);;Text Files (*.txt)")
        if file_name:
            self.labelInfo.setText(f"Loaded file: {file_name}")
            self.ChannelDatas = read_and_process_data(file_name) # 8채널 데이터 읽기 및 처리    
            
            channel_0 = self.ChannelDatas[:,0]
            
            print(f"data length: {len(channel_0)}")
            # 데이터 플로팅
            self.plot_widget.clear()  # 기존 플롯 지우기
            # self.plot_widget.plot(len(channel_0),channel_0, pen='b')  # 파란색 선으로 플로팅
            
            x = np.arange(len(channel_0))
            self.plot_widget.plot(x, channel_0, pen='b')

                
            
            
            
#개발 테스트
if __name__ == "__main__":
    app = QtWidgets.QApplication([])
    window = DataForm()
    window.show()
    sys.exit(app.exec())