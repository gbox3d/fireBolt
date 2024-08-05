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
        self.btnFindNext.clicked.connect(self.onClickFindNextBtn)
        
        # 위젯 참조
        self.labelInfo = self.findChild(QtWidgets.QLabel, 'labelInfo')
        
        # PyQtGraph 위젯 설정
        self.setup_line_graph()
    
        self.current_index = 0
        self.current_index_line = None  # 현재 인덱스를 표시할 선 객체
        
        self.ChannelDatas = None  # 데이터 저장 변수
        
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
        
         # 뷰 범위가 변경될 때마다 호출될 콜백 함수 설정
        self.plot_widget.getViewBox().sigRangeChanged.connect(self.onRangeChanged)
        
    def updateCurrentIndexLine(self):
        if self.current_index_line:
            self.plot_widget.removeItem(self.current_index_line)
        self.current_index_line = pg.InfiniteLine(pos=self.current_index, angle=90, pen=pg.mkPen('g', width=2))
        self.plot_widget.addItem(self.current_index_line)
        self.labelCurIndex.setText(f"Current Index: {self.current_index}")    
    def onRangeChanged(self):
        if self.ChannelDatas is not None:
            x_min, x_max = self.plot_widget.viewRange()[0]
            self.current_index = int((x_min + x_max) / 2)
            self.updateCurrentIndexLine()
    
    
        
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
            
            # 초기 current_index 설정
            self.current_index = 0
            
            
    def findNextData(self,channalIndex,currentIndex):
        # 1 현재부터 검사하여 0이 아닌 데이터를 찾음
        
        _data = self.ChannelDatas[:,channalIndex]
        
        for i in range(currentIndex,len(_data)):
            if _data[i] != 0:
                return i
            
        return -1
    
    def onClickFindNextBtn(self):
        _index = self.findNextData(0, self.current_index)
        if _index != -1:
            print(f"find next data index: {_index}")
            self.current_index = _index  # 현재 위치 업데이트
            self.scrollToIndex(_index)
        else:
            print("No more non-zero data found")

    def scrollToIndex(self, index):
        # 그래프의 현재 X 범위를 가져옵니다
        x_min, x_max = self.plot_widget.viewRange()[0]
        range_width = x_max - x_min

        # 새로운 범위를 계산합니다
        new_min = max(0, index - range_width / 2)
        new_max = new_min + range_width

        # 그래프의 X 범위를 업데이트합니다
        self.plot_widget.setXRange(new_min, new_max, padding=0)
        
        self.updateCurrentIndexLine()
            
            
#개발 테스트
if __name__ == "__main__":
    app = QtWidgets.QApplication([])
    window = DataForm()
    window.show()
    sys.exit(app.exec())