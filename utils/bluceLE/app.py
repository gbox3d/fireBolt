import sys
from PySide6.QtWidgets import (QApplication, QWidget, QVBoxLayout, QHBoxLayout, 
                               QLabel, QPushButton, QTableWidget, QTableWidgetItem, QHeaderView, QSpacerItem, QSizePolicy,
                               QMessageBox)
from PySide6.QtCore import Qt, QTimer
from PySide6.QtGui import QFont
from PySide6.QtBluetooth import QBluetoothDeviceDiscoveryAgent, QBluetoothDeviceInfo, QLowEnergyController

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        
        self.discovered_devices = []  # 발견된 디바이스 정보를 저장할 리스트
        
        self.initUI()
        self.initBLE()
    
    def initUI(self):
        self.setGeometry(100, 100, 800, 600)
        self.setWindowTitle('The BluceLE Central Test Tool')

        main_layout = QVBoxLayout()
        
        # 버튼 바 (수평 레이아웃) 추가
        button_layout = QHBoxLayout()
        
         # 스캔 버튼 추가
        self.scan_button = QPushButton("Scan for Peripherals")
        self.scan_button.clicked.connect(self.toggle_scan)
        button_layout.addWidget(self.scan_button)
        
        # 연결 버튼 추가
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.onClick_connect)
        button_layout.addWidget(self.connect_button)
        
        # 수평 스페이서 추가 (버튼을 왼쪽으로 밀어냄)
        button_layout.addSpacerItem(QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum))

        # 버튼 레이아웃을 메인 레이아웃에 추가
        main_layout.addLayout(button_layout)

        # 테이블 위젯 추가
        self.device_table = QTableWidget()
        self.device_table.setColumnCount(6)
        self.device_table.setHorizontalHeaderLabels(['Name', 'Address', 'Type', 'RSSI', 'Manufacturer Data', 'Service UUIDs'])
        self.device_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.device_table.setFixedHeight(200)  # 높이를 400으로 고정
        self.device_table.setFont(QFont("Arial", 9))
        # 테이블 위젯에 선택 변경 이벤트 연결
        self.device_table.itemSelectionChanged.connect(self.update_connect_button)
        
        main_layout.addWidget(self.device_table)
        
         # 수직 스페이서 추가 (테이블 아래 공간을 채움)
        main_layout.addSpacerItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding))


        self.setLayout(main_layout)

    def initBLE(self):
        self.discovery_agent = QBluetoothDeviceDiscoveryAgent()
        self.discovery_agent.deviceDiscovered.connect(self.add_device)
        self.discovery_agent.finished.connect(self.scan_finished)
         # 스캔 타임아웃 설정 (10초)
        self.discovery_agent.setLowEnergyDiscoveryTimeout(10000)
        
    def toggle_scan(self):
        if self.discovery_agent.isActive():
            self.stop_scan()
        else:
            self.start_scan()

    def start_scan(self):
        self.device_table.setRowCount(0)  # 테이블 초기화
        self.discovery_agent.start(QBluetoothDeviceDiscoveryAgent.LowEnergyMethod)
        self.scan_button.setText("Stop Scan")
        self.connect_button.setEnabled(False)  # 스캔 중에는 연결 버튼 비활성화
        
    
    def stop_scan(self):
        self.discovery_agent.stop()
        self.scan_button.setText("Scan for Peripherals")
    
    def add_device(self, device_info):
        if device_info not in self.discovered_devices:
            self.discovered_devices.append(device_info)
            row_position = self.device_table.rowCount()
            self.device_table.insertRow(row_position)

            device_name = device_info.name() or "Unknown Device"
            device_address = device_info.address().toString()
            rssi = device_info.rssi()
            
            # 장치 타입 확인
            device_type = "Unknown"
            if device_info.coreConfigurations() & QBluetoothDeviceInfo.LowEnergyCoreConfiguration:
                device_type = "BLE"
            elif device_info.coreConfigurations() & QBluetoothDeviceInfo.BaseRateCoreConfiguration:
                device_type = "Classic"
            
            # 제조사 데이터 확인 (예외 처리 추가)
            manufacturer_data = device_info.manufacturerData()
            manufacturer_info = []
            for key, value in manufacturer_data.items():
                try:
                    if isinstance(value, (bytes, bytearray)):
                        manufacturer_info.append(f"{key}: {value.hex()}")
                    elif isinstance(value, list):
                        manufacturer_info.append(f"{key}: {', '.join(map(str, value))}")
                    else:
                        manufacturer_info.append(f"{key}: {str(value)}")
                except Exception as e:
                    manufacturer_info.append(f"{key}: Error ({str(e)})")
            manufacturer_info = ", ".join(manufacturer_info)
            
            # 서비스 UUID 확인
            try:
                service_uuids = ", ".join([str(uuid) for uuid in device_info.serviceUuids()])
            except Exception as e:
                service_uuids = f"Error retrieving UUIDs: {str(e)}"
            
            # 테이블에 디바이스 정보 추가
            items = [
                QTableWidgetItem(device_name),
                QTableWidgetItem(device_address),
                QTableWidgetItem(device_type),
                QTableWidgetItem(str(rssi)),
                QTableWidgetItem(manufacturer_info),
                QTableWidgetItem(service_uuids)
            ]
            
            for col, item in enumerate(items):
                self.device_table.setItem(row_position, col, item)
    
    
    def onClick_connect(self):
        print("Connect button clicked")
        if self.connect_button.text() == "Connect":
            self.connect_button.setText("Disconnect")
            self.connect_device()
        else:
            self.connect_button.setText("Connect")
            self.disconnect_device()
    
    def connect_device(self):
        selected_items = self.device_table.selectedItems()
        if not selected_items:
            QMessageBox.warning(self, "Warning", "Please select a device to connect.")
            return

        selected_row = selected_items[0].row()
        if selected_row < 0 or selected_row >= len(self.discovered_devices):
            QMessageBox.warning(self, "Warning", "Invalid device selection.")
            return

        device_info = self.discovered_devices[selected_row]
        
        # 연결 프로세스 시작
        self.ble_controller = QLowEnergyController.createCentral(device_info)
        self.ble_controller.connected.connect(self.device_connected)
        self.ble_controller.disconnected.connect(self.device_disconnected)
        self.ble_controller.errorOccurred.connect(self.connection_error)

        print(f"Attempting to connect to {device_info.name()} ({device_info.address().toString()})")
        self.ble_controller.connectToDevice()
    def disconnect_device(self):
        if self.ble_controller:
            self.ble_controller.disconnectFromDevice()
            self.ble_controller = None
            print("Disconnected from device")
            QMessageBox.information(self, "Disconnected", "Disconnected from device.")
        else:
            print("No device to disconnect")
            QMessageBox.warning(self, "Warning", "No device to disconnect.")
        
    def device_connected(self):
        print("Device connected successfully!")
        QMessageBox.information(self, "Success", "Device connected successfully!")
        
        self.connect_button.setText("Disconnect")

    def device_disconnected(self):
        print("Device disconnected")
        QMessageBox.information(self, "Disconnected", "Device has been disconnected.")

    def connection_error(self, error):
        error_message = f"Connection error: {error}"
        print(error_message)
        QMessageBox.critical(self, "Error", error_message)
    def update_connect_button(self):
        self.connect_button.setEnabled(len(self.device_table.selectedItems()) > 0)
    
    def scan_finished(self):
        print("Scan finished")
        self.stop_scan()

    def closeEvent(self, event):
        print("closeEvent")
        super().closeEvent(event)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())