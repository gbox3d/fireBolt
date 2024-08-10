# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'dataProcBasic.ui'
##
## Created by: Qt User Interface Compiler version 6.7.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QHBoxLayout, QLabel, QPushButton,
    QSizePolicy, QSpacerItem, QWidget)

class Ui_Form(object):
    def setupUi(self, Form):
        if not Form.objectName():
            Form.setObjectName(u"Form")
        Form.resize(757, 632)
        self.widgetLineGraphContainer = QWidget(Form)
        self.widgetLineGraphContainer.setObjectName(u"widgetLineGraphContainer")
        self.widgetLineGraphContainer.setGeometry(QRect(20, 120, 711, 251))
        self.horizontalLayoutWidget = QWidget(Form)
        self.horizontalLayoutWidget.setObjectName(u"horizontalLayoutWidget")
        self.horizontalLayoutWidget.setGeometry(QRect(10, 10, 691, 32))
        self.horizontalLayout = QHBoxLayout(self.horizontalLayoutWidget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.btnLoadData = QPushButton(self.horizontalLayoutWidget)
        self.btnLoadData.setObjectName(u"btnLoadData")

        self.horizontalLayout.addWidget(self.btnLoadData)

        self.btnFindNext = QPushButton(self.horizontalLayoutWidget)
        self.btnFindNext.setObjectName(u"btnFindNext")

        self.horizontalLayout.addWidget(self.btnFindNext)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.labelInfo = QLabel(Form)
        self.labelInfo.setObjectName(u"labelInfo")
        self.labelInfo.setGeometry(QRect(20, 70, 631, 16))
        self.labelCurIndex = QLabel(Form)
        self.labelCurIndex.setObjectName(u"labelCurIndex")
        self.labelCurIndex.setGeometry(QRect(20, 90, 201, 16))

        self.retranslateUi(Form)

        QMetaObject.connectSlotsByName(Form)
    # setupUi

    def retranslateUi(self, Form):
        Form.setWindowTitle(QCoreApplication.translate("Form", u"Basic DataProcessing", None))
        self.btnLoadData.setText(QCoreApplication.translate("Form", u"Load", None))
        self.btnFindNext.setText(QCoreApplication.translate("Form", u"FindNextData", None))
        self.labelInfo.setText(QCoreApplication.translate("Form", u"info text", None))
        self.labelCurIndex.setText(QCoreApplication.translate("Form", u"current index", None))
    # retranslateUi

