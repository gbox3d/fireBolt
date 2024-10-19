import sys
from PySide6.QtWidgets import (QApplication, QWidget, QVBoxLayout, QHBoxLayout, 
                               QLabel, QPushButton, QTableWidget, QTableWidgetItem, QHeaderView, QSpacerItem, QSizePolicy,
                               QMessageBox,QTextEdit,QLineEdit)
from PySide6.QtCore import Qt, QTimer
from PySide6.QtGui import QFont

from PySide6.QtBluetooth import QBluetoothDeviceDiscoveryAgent, QBluetoothDeviceInfo, QLowEnergyController, QLowEnergyService, QLowEnergyCharacteristic,QBluetoothUuid

  
  
class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        
        self.discovered_devices = []  # 발견된 디바이스 정보를 저장할 리스트
        
        self.target_service_uuid = "457556f0-842a-41ac-b777-cb4af6f47720"
        self.target_characteristic_uuid = "3c3ffbea-1856-460a-b0dd-b8a2a3a8352b"
        self.ble_controller = None
        self.ble_service = None
        self.ble_characteristic = None
        
        self.load_uuids_from_file()  # UUID 파일에서 값을 로드
        
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
        
        # ble reset 버튼 추가
        self.reset_button = QPushButton("Reset BLE")
        self.reset_button.clicked.connect(self.onClickResetBle)
        button_layout.addWidget(self.reset_button)
        
        # 수평 스페이서 추가 (버튼을 왼쪽으로 밀어냄)
        button_layout.addSpacerItem(QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum))

        # 버튼 레이아웃을 메인 레이아웃에 추가
        main_layout.addLayout(button_layout)
        
        
        
        # uuid 위제 추가
        uuid_layout = QHBoxLayout()
        self.service_uuid_label = QLabel("Service UUID:")
        uuid_layout.addWidget(self.service_uuid_label)
        self.service_uuid_le = QLineEdit()
        self.service_uuid_le.setText(self.target_service_uuid)
        uuid_layout.addWidget(self.service_uuid_le)
        
        self.characteristic_uuid_label = QLabel("Characteristic UUID:")
        uuid_layout.addWidget(self.characteristic_uuid_label)
        self.characteristic_uuid_le = QLineEdit()
        self.characteristic_uuid_le.setText(self.target_characteristic_uuid)
        uuid_layout.addWidget(self.characteristic_uuid_le)
        
        main_layout.addLayout(uuid_layout)

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
        
        # 데이터 표시 위젯 추가
        self.data_display = QTextEdit()
        self.data_display.setReadOnly(True)
        self.data_display.setFont(QFont("Courier", 10))
        self.data_display.setFixedHeight(200)  # 높이를 200으로 고정
        main_layout.addWidget(self.data_display)
        
        # 커멘드 입력 위젯 추가
        cmdLayout = QHBoxLayout()
        
        self.cmdInput = QLineEdit()
        self.cmdInput.setPlaceholderText("Enter command")
        cmdLayout.addWidget(self.cmdInput)
        
        self.btnSend = QPushButton("Send")
        self.btnSend.clicked.connect(self.send_command)
        cmdLayout.addWidget(self.btnSend)
        
        main_layout.addLayout(cmdLayout)
        
                
         # 수직 스페이서 추가 (테이블 아래 공간을 채움)
        main_layout.addSpacerItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding))


        self.setLayout(main_layout)
        
    def send_command(self):
        if not self.ble_characteristic or not self.ble_service:
            QMessageBox.warning(self, "Error", "No characteristic selected.")
            return
        
        cmd = self.cmdInput.text()
        if not cmd:
            QMessageBox.warning(self, "Error", "Please enter a command.")
            return
        
        cmd_bytes = bytes(cmd, 'utf-8')
        self.ble_service.writeCharacteristic(self.ble_characteristic, cmd_bytes)
        self.log(f"Sent command: {cmd}")
        self.cmdInput.clear()

    def initBLE(self):
        self.discovery_agent = QBluetoothDeviceDiscoveryAgent()
        self.discovery_agent.deviceDiscovered.connect(self.add_device)
        self.discovery_agent.finished.connect(self.scan_finished)
         # 스캔 타임아웃 설정 (10초)
        self.discovery_agent.setLowEnergyDiscoveryTimeout(10000)
        
    def onClickResetBle(self):
        
        self.initBLE()
        self.discovered_devices = []
        print("Device reset")
        
        
    def toggle_scan(self):
        if self.discovery_agent.isActive():
            self.stop_scan()
        else:
            self.start_scan()

    def start_scan(self):
        
        self.discovered_devices = []  # 발견된 디바이스 정보 초기화
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
        # print("Connect button clicked")
        # 현재 설정 저장
        self.target_service_uuid = self.service_uuid_le.text()
        self.target_characteristic_uuid = self.characteristic_uuid_le.text()
        self.save_uuids_to_file()
            
        if self.connect_button.text() == "Connect":
            self.connect_button.setText("Disconnect")
            self.connect_device()
        else:
            self.connect_button.setText("Connect")
            self.disconnect_device()
    
    def load_uuids_from_file(self):
        try:
            with open('uuid.txt', 'r') as file:
                lines = file.readlines()
                if len(lines) >= 2:
                    self.target_service_uuid = lines[0].strip()
                    self.target_characteristic_uuid = lines[1].strip()
        except FileNotFoundError:
            # 파일이 없으면 기본값 사용
            print("UUID file not found,use default values")
            pass
    def save_uuids_to_file(self):
        with open('uuid.txt', 'w') as file:
            file.write(f"{self.target_service_uuid}\n")
            file.write(f"{self.target_characteristic_uuid}\n")
         
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
        
        self.ble_controller = QLowEnergyController.createCentral(device_info)
        self.ble_controller.connected.connect(self.device_connected)
        self.ble_controller.disconnected.connect(self.device_disconnected)
        self.ble_controller.errorOccurred.connect(self.connection_error)
        self.ble_controller.serviceDiscovered.connect(self.service_discovered) # 서비스 발견 시 호출
        self.ble_controller.discoveryFinished.connect(self.service_scan_done) # 서비스 스캔 완료 시 호출

        self.log(f"Attempting to connect to {device_info.name()} ({device_info.address().toString()})")
        self.ble_controller.connectToDevice()

    def disconnect_device(self):
        if self.ble_controller:
            self.ble_controller.disconnectFromDevice()
            self.ble_controller = None
        else:
            print("No device to disconnect")
    
    def device_connected(self):
        self.log("Device connected successfully!")
        # QMessageBox.information(self, "Success", "Device connected successfully!")
        self.connect_button.setText("Disconnect")
        self.log("Starting service discovery...")
        self.ble_controller.discoverServices()

    def device_disconnected(self):
        print("Device disconnected")
        QMessageBox.information(self, "Disconnected", "Device has been disconnected.")

    def connection_error(self, error):
        error_message = f"Connection error: {error}"
        print(error_message)
        QMessageBox.critical(self, "Error", error_message)
    def update_connect_button(self):
        self.connect_button.setEnabled(len(self.device_table.selectedItems()) > 0)
        
    def service_discovered(self, service_uuid):
        self.log(f"Service discovered: {service_uuid}")
        
    def service_scan_done(self):
        self.log("Service scan completed")
        services = self.ble_controller.services()
        for service in services:
            
            if service.toString()[1:-1] == self.target_service_uuid:
            
                ble_service = self.ble_controller.createServiceObject(service)
                if ble_service:
                    ble_service.stateChanged.connect(self.service_state_changed)
                    ble_service.characteristicChanged.connect(self.characteristic_changed)
                    ble_service.characteristicRead.connect(self.characteristic_read)
                    self.log(f"Discovering details for service: {service.toString()}")
                    ble_service.discoverDetails()
                self.ble_service = ble_service
                self.log("Found the desired service")
                break
            
        if not self.ble_service:
            self.log("Error: Could not find the desired service")
    
    def service_state_changed(self, state):
        if state == QLowEnergyService.ServiceDiscovered:
            
            service = self.ble_service
            self.log(F"Service details discovered : uuid={self.ble_service.serviceUuid().toString()}")
            characteristics = service.characteristics()
            
            for char in characteristics:
                self.log(f"Found characteristic: {char.uuid().toString()}")
                
                # {...} 에서 대괄호만 제거
                uuid = char.uuid().toString()
                uuid = uuid[1:-1]
                
                if uuid == self.target_characteristic_uuid:
                
                    self.ble_characteristic = char
                    cccd_uuid = QBluetoothUuid(QBluetoothUuid.DescriptorType.ClientCharacteristicConfiguration)
                    descriptor = char.descriptor(cccd_uuid)
                    if descriptor.isValid():
                        self.log(f"Enabling notifications for characteristic: {char.uuid().toString()}")
                        self.ble_service.writeDescriptor(descriptor, bytes([0x01, 0x00]))
                        self.ble_service.readCharacteristic(char) # Read the characteristic value
                        # QMessageBox.information(self, "Success", "Connected to device successfully!")
                    else:
                        self.log(f"Invalid descriptor for characteristic: {char.uuid().toString()}")
                
    def characteristic_changed(self, characteristic, value):
        hex_data = self.byte_array_to_hex(value)
        ascii_data = self.byte_array_to_ascii(value)
        display_text = f"Received: {hex_data} (ASCII: {ascii_data})"
        # self.log(display_text)
        self.data_display.append(display_text)
        
    def characteristic_read(self, characteristic, value):
        hex_data = self.byte_array_to_hex(value)
        ascii_data = self.byte_array_to_ascii(value)
        display_text = f"Read: {hex_data} (ASCII: {ascii_data})"
        # self.log(display_text)
        self.data_display.append(display_text)

    def byte_array_to_hex(self, value):
        return value.toHex().data().hex()

    def byte_array_to_ascii(self, value):
        return ''.join([chr(b) if 32 <= b <= 126 else '.' for b in value.data()])
        
    def log(self, message):
        print(message)
        self.data_display.append(f"[LOG] {message}")    
    
        
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