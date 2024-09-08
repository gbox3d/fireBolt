import sys
from PySide6.QtWidgets import QApplication, QMainWindow, QMessageBox
from PySide6.QtCore import Slot
from serial_handler import SerialThread, scan_serial_ports

from UI.mainWindow import Ui_MainWindow

from optionDialog import OptionDialog

class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.serial_thread = None

        self.actionScan.triggered.connect(self.scan_serial_ports)
        self.actionOption.triggered.connect(self.open_option_dialog)
        self.btnConnect.clicked.connect(self.connect_serial_port)
        self.btnSend.clicked.connect(self.send_data)

    @Slot()
    def scan_serial_ports(self):
        """Scan for available serial ports and update the combo box."""
        ports = scan_serial_ports()
        self.cbPortList.clear()  # Clear the combo box before populating
        for port in ports:
            self.cbPortList.addItem(port.device)

        if not ports:
            QMessageBox.information(self, "No Ports Found", "No serial ports were found.")

    @Slot()
    def connect_serial_port(self):
        """Connect to or disconnect from the selected serial port."""
        
        # If already connected, disconnect
        if self.serial_thread and self.serial_thread.isRunning():
            self.serial_thread.stop()
            self.serial_thread.wait()
            self.serial_thread = None
            
            QMessageBox.information(self, "Disconnection", "Disconnected from serial port.")
            
            # Change button text back to "Connect"
            self.btnConnect.setText("Connect")
            
            return

        port_name = self.cbPortList.currentText()
        if port_name:
            try:
                # Connect to the serial port
                self.serial_thread = SerialThread( port_name= port_name,baudrate=115200 )
                self.serial_thread.data_received.connect(self.update_received_data)
                self.serial_thread.start()
                QMessageBox.information(self, "Connection", f"Connected to {port_name}")
                
                # Change button text to "Disconnect"
                self.btnConnect.setText("Disconnect")
                
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Could not open serial port: {e}")
        else:
            QMessageBox.warning(self, "Warning", "Please select a serial port.")

    @Slot()
    def send_data(self):
        """Send data through the serial port."""
        if self.serial_thread and self.serial_thread.serial_port.is_open:
            data = self.leReqData.text()
            if data:
                self.serial_thread.serial_port.write(data.encode('utf-8'))
            else:
                QMessageBox.warning(self, "Warning", "Please enter data to send.")
        else:
            QMessageBox.warning(self, "Warning", "No serial port connected.")
            
    @Slot()
    def open_option_dialog(self):
        """Open the option dialog."""
        _dlg = OptionDialog()
        if _dlg.exec() == OptionDialog.Accepted:
            baudrate = _dlg.get_baudrate()
            print(f"Selected Baudrate: {baudrate}")
        else:
            print("Dialog was cancelled.")
        
        

    @Slot(str)
    def update_received_data(self, data):
        """Update the QTextEdit with received data."""
        self.teRecvData.append(data)

    def closeEvent(self, event):
        """Handle the window close event."""
        if self.serial_thread and self.serial_thread.isRunning():
            self.serial_thread.stop()
            self.serial_thread.wait()
        event.accept()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
