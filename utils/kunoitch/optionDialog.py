from PySide6 import QtWidgets
from PySide6.QtCore import Qt
from UI.optionDlg import Ui_Dialog as OptionDialog

class OptionDialog(QtWidgets.QDialog, OptionDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setupUi(self)
        
        # Connect signals and slots
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

        # Optionally set default values
        self.set_default_values()

    def set_default_values(self):
        """Set default values for the dialog."""
        self.lineEdit.setText("9600")  # Set default baudrate to 9600

    def get_baudrate(self):
        """Retrieve the baudrate from the dialog."""
        return self.lineEdit.text()

    def accept(self):
        """Handle the accept event."""
        # Validate the input, if necessary
        baudrate = self.get_baudrate()
        if not baudrate.isdigit():
            QtWidgets.QMessageBox.warning(self, "Invalid Input", "Baudrate must be a number.")
            return

        super().accept()  # Call the superclass's accept method to close the dialog

    def reject(self):
        """Handle the reject event."""
        super().reject()  # Simply call the superclass's reject method

# Example usage of the dialog in a main application window
if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    dialog = OptionDialog()
    if dialog.exec() == QtWidgets.QDialog.Accepted:
        baudrate = dialog.get_baudrate()
        print(f"Selected Baudrate: {baudrate}")
    else:
        print("Dialog was cancelled.")
    sys.exit(app.exec())
