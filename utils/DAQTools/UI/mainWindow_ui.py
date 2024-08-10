# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'mainwindow.ui'
##
## Created by: Qt User Interface Compiler version 6.7.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QFrame, QHBoxLayout, QLabel,
    QLineEdit, QMainWindow, QMenu, QMenuBar,
    QPushButton, QSizePolicy, QSpacerItem, QStatusBar,
    QVBoxLayout, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(800, 597)
        self.actionAbout = QAction(MainWindow)
        self.actionAbout.setObjectName(u"actionAbout")
        self.DataProcessBasicForm = QAction(MainWindow)
        self.DataProcessBasicForm.setObjectName(u"DataProcessBasicForm")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self._2 = QVBoxLayout(self.centralwidget)
        self._2.setObjectName(u"_2")
        self.frame = QFrame(self.centralwidget)
        self.frame.setObjectName(u"frame")
        self.frame.setFrameShape(QFrame.Shape.StyledPanel)
        self.frame.setFrameShadow(QFrame.Shadow.Raised)
        self.layoutWidget_2 = QWidget(self.frame)
        self.layoutWidget_2.setObjectName(u"layoutWidget_2")
        self.layoutWidget_2.setGeometry(QRect(10, 10, 318, 24))
        self.AddressInputBar = QHBoxLayout(self.layoutWidget_2)
        self.AddressInputBar.setObjectName(u"AddressInputBar")
        self.AddressInputBar.setContentsMargins(0, 0, 0, 0)
        self.label = QLabel(self.layoutWidget_2)
        self.label.setObjectName(u"label")

        self.AddressInputBar.addWidget(self.label)

        self.lineEdit_IP = QLineEdit(self.layoutWidget_2)
        self.lineEdit_IP.setObjectName(u"lineEdit_IP")

        self.AddressInputBar.addWidget(self.lineEdit_IP)

        self.label_2 = QLabel(self.layoutWidget_2)
        self.label_2.setObjectName(u"label_2")

        self.AddressInputBar.addWidget(self.label_2)

        self.lineEdit_Port = QLineEdit(self.layoutWidget_2)
        self.lineEdit_Port.setObjectName(u"lineEdit_Port")

        self.AddressInputBar.addWidget(self.lineEdit_Port)

        self.graphContainer = QWidget(self.frame)
        self.graphContainer.setObjectName(u"graphContainer")
        self.graphContainer.setGeometry(QRect(10, 110, 751, 221))
        self.layoutWidget = QWidget(self.frame)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(10, 50, 761, 52))
        self.MenuBttonBar = QHBoxLayout(self.layoutWidget)
        self.MenuBttonBar.setObjectName(u"MenuBttonBar")
        self.MenuBttonBar.setContentsMargins(0, 0, 0, 0)
        self.btnConnect = QPushButton(self.layoutWidget)
        self.btnConnect.setObjectName(u"btnConnect")

        self.MenuBttonBar.addWidget(self.btnConnect)

        self.btnPing = QPushButton(self.layoutWidget)
        self.btnPing.setObjectName(u"btnPing")

        self.MenuBttonBar.addWidget(self.btnPing)

        self.btn_startDAQ = QPushButton(self.layoutWidget)
        self.btn_startDAQ.setObjectName(u"btn_startDAQ")

        self.MenuBttonBar.addWidget(self.btn_startDAQ)

        self.btnFindDevice = QPushButton(self.layoutWidget)
        self.btnFindDevice.setObjectName(u"btnFindDevice")

        self.MenuBttonBar.addWidget(self.btnFindDevice)

        self.btnCapture = QPushButton(self.layoutWidget)
        self.btnCapture.setObjectName(u"btnCapture")

        self.MenuBttonBar.addWidget(self.btnCapture)

        self.horizontalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.MenuBttonBar.addItem(self.horizontalSpacer)

        self.labelInfo = QLabel(self.frame)
        self.labelInfo.setObjectName(u"labelInfo")
        self.labelInfo.setGeometry(QRect(10, 350, 771, 20))
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelInfo.sizePolicy().hasHeightForWidth())
        self.labelInfo.setSizePolicy(sizePolicy)

        self._2.addWidget(self.frame)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 800, 43))
        self.menuHelp = QMenu(self.menubar)
        self.menuHelp.setObjectName(u"menuHelp")
        self.menuData_Process = QMenu(self.menubar)
        self.menuData_Process.setObjectName(u"menuData_Process")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuHelp.menuAction())
        self.menubar.addAction(self.menuData_Process.menuAction())
        self.menuHelp.addAction(self.actionAbout)
        self.menuData_Process.addAction(self.DataProcessBasicForm)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"DAQ Tool", None))
        self.actionAbout.setText(QCoreApplication.translate("MainWindow", u"About", None))
        self.DataProcessBasicForm.setText(QCoreApplication.translate("MainWindow", u"basic", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"IP", None))
        self.lineEdit_IP.setPlaceholderText(QCoreApplication.translate("MainWindow", u"IP Address", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"Port", None))
        self.lineEdit_Port.setPlaceholderText(QCoreApplication.translate("MainWindow", u"Port", None))
        self.btnConnect.setText(QCoreApplication.translate("MainWindow", u"Connect", None))
        self.btnPing.setText(QCoreApplication.translate("MainWindow", u"Ping", None))
        self.btn_startDAQ.setText(QCoreApplication.translate("MainWindow", u"Start DAQ", None))
        self.btnFindDevice.setText(QCoreApplication.translate("MainWindow", u"findDevice", None))
        self.btnCapture.setText(QCoreApplication.translate("MainWindow", u"Start Capture", None))
        self.labelInfo.setText(QCoreApplication.translate("MainWindow", u"Ready", None))
        self.menuHelp.setTitle(QCoreApplication.translate("MainWindow", u"Help", None))
        self.menuData_Process.setTitle(QCoreApplication.translate("MainWindow", u"Data Process", None))
    # retranslateUi

