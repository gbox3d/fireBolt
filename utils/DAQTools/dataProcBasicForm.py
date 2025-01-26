import sys
# from PyQt6 import QtWidgets, uic
# from PyQt6.QtCore import QFile
# from PyQt6.QtCore import QFile, QThread, pyqtSignal

from PySide6.QtWidgets import QFileDialog, QWidget,QLabel,QVBoxLayout,QApplication
import pyqtgraph as pg

from collections import deque

# data_form.py


# from PyQt6 import QtWidgets, uic
# from PyQt6.QtWidgets import QFileDialog

from procData import read_and_process_data

import numpy as np

import UI.dataProcBasic_ui


class DataForm(QWidget, UI.dataProcBasic_ui.Ui_Form):
    def __init__(self):
        super().__init__()
        
        self.setupUi(self)
        
        # UI 파일 로드
        # uic.loadUi('dataProcBasic.ui', self)
        
        # 버튼 연결
        self.btnLoadData.clicked.connect(self.load_data)
        self.btnFindNext.clicked.connect(self.onClickFindNextBtn)
        self.btnCal.clicked.connect(self.onCalculated)
        
        # 위젯 참조
        self.labelInfo = self.findChild(QLabel, 'labelInfo')
        
        # PyQtGraph 위젯 설정
        self.setup_line_graph()
    
        self.current_index = 0
        self.current_index_line = None  # 현재 인덱스를 표시할 선 객체
        
        self.ChannelDatas = None  # 데이터 저장 변수
        
    def setup_line_graph(self):
        # PyQtGraph 위젯 생성
        self.plot_widget = pg.PlotWidget()
        
        # widgetLineGraphContainer에 그래프 추가
        layout = QVBoxLayout(self.widgetLineGraphContainer)
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
            
    def measure_signal_length(self,channel_data, start_idx, max_length=500):
        """
        신호의 길이를 측정합니다.

        Args:
            channel_data: 채널 데이터 배열 (0/1 값)
            start_idx: 신호가 시작된 인덱스
            max_length: 최소 0이 지속되어야 하는 길이 (기본값: 500)

        Returns:
            int: 신호의 길이 (샘플 개수 기준)
        """
        length = 0
        end_idx = len(channel_data)

        # 신호가 시작된 이후로 탐색
        for i in range(start_idx, end_idx):
            if channel_data[i] == 0:
                # 0이 지속되는지 확인
                if i + max_length <= end_idx and all(channel_data[i:i + max_length] == 0):
                    # 500개의 0이 연속되면 끝으로 간주
                    break
            length += 1

        return length

            
    def onCalculated(self):
        """
        현재 인덱스(self.current_index) 이후 각 채널에서 0이 아닌 첫 번째 신호가 나타나는 인덱스를 계산하고,
        그 시차(초)를 출력 + 신호 길이를 측정.
        """
        if self.ChannelDatas is None:
            print("No data loaded.")
            return
        
        self.teLogText.clear()
        
        # 샘플링 레이트 설정
        sampling_rate = 24000  # Hz
        time_per_sample = 1 / sampling_rate  # 초 단위 시간 간격

        # 데이터 정보
        num_samples, num_channels = self.ChannelDatas.shape

        # 결과 저장
        results = []

        for ch in range(num_channels):
            # 현재 인덱스 이후 데이터를 탐색 (채널별 slice)
            channel_data = self.ChannelDatas[self.current_index:, ch]

            # 0이 아닌 값의 상대 인덱스들
            non_zero_indices = np.where(channel_data != 0)[0]

            if len(non_zero_indices) > 0:
                # 첫 번째 신호 시작 (절대 인덱스)
                first_index = self.current_index + non_zero_indices[0]
                
                # 신호 길이(샘플 단위) 계산
                signal_len_samples = self.measure_signal_length(
                    channel_data,    # 해당 채널의 current_index 이후 slice
                    non_zero_indices[0]  # slice 내부에서의 시작 위치
                
                )
                
                # 신호 길이(초 단위)
                signal_len_seconds = signal_len_samples * time_per_sample

                # 첫 신호까지의 시차(초)
                time_difference = (first_index - self.current_index) * time_per_sample

                results.append((ch, first_index, time_difference, signal_len_samples, signal_len_seconds))
            else:
                # 해당 채널에 신호가 없음
                results.append((ch, None, None, 0, 0.0))

        # 결과 출력
        print(f"Current Index: {self.current_index}")
        for ch, first_index, time_diff, sig_len_samples, sig_len_seconds in results:
            if first_index is not None:
                print(
                    f"Channel {ch}: First signal at index {first_index}"
                    f" (delay={time_diff:.6f}s), length={sig_len_samples} samples ({sig_len_seconds:.6f}s)"
                )
                self.teLogText.appendPlainText(f"Channel {ch}: First signal at index {first_index} (delay={time_diff:.6f}s), length={sig_len_samples} samples ({sig_len_seconds:.6f}s)")
            else:
                print(f"Channel {ch}: No signal found")

        
    def load_data(self):
        # 파일 로드 다이얼로그
        file_name, _ = QFileDialog.getOpenFileName(self, "Open File", "", "All Files (*);;Text Files (*.txt)")
        if file_name:
            self.labelInfo.setText(f"Loaded file: {file_name}")
            
            # 8채널 데이터 읽기 및 처리
            self.ChannelDatas = read_and_process_data(file_name)  # shape: (N, 8)
            
            # 데이터 길이 확인
            num_samples, num_channels = self.ChannelDatas.shape
            print(f"Data loaded: {num_samples} samples, {num_channels} channels")
            
            # 기존 플롯 초기화
            self.plot_widget.clear()
            
            # 채널별로 그래프 그리기
            x = np.arange(num_samples)  # X축: 인덱스
            colors = ['r', 'g', 'b', 'c', 'm', 'y', 'k', 'orange']  # 채널별 색상 (필요 시 확장)
            
            for ch in range(num_channels):
                y = self.ChannelDatas[:, ch] + ch  # Y축: 채널 데이터 + 오프셋
                color = colors[ch % len(colors)]  # 색상 순환
                self.plot_widget.plot(x, y, pen=color, name=f"Channel {ch}")
            
            # 그래프 Y축 범위 설정 (모든 채널을 포함하도록)
            self.plot_widget.setYRange(-0.5, num_channels + 0.5, padding=0.1)
            
            # 초기 current_index 설정
            self.current_index = 0
            self.updateCurrentIndexLine()

            
            
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
    app = QApplication([])
    window = DataForm()
    window.show()
    sys.exit(app.exec())