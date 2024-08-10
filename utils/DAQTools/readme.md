# DAQ Tools

## Description
This repository contains a collection of tools for data acquisition (DAQ) and data analysis. The tools are written in Python and are based on the [PyQt5](https://pypi.org/project/PyQt5/) library for the graphical user interface (GUI).

## Installation

### Requirements
```bash
pip install -r requirements.txt

```

## Development

compile the .ui file to a .py file using the following command:
```bash
pyside6-uic mainwindow.ui -o mainWindow_ui.py
pyside6-uic dataProcBasic.ui -o dataProcBasic_ui.py
pyside6-uic findDeviceDlg.ui -o findDeviceDlg_ui.py
```