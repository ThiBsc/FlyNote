#-------------------------------------------------
#
# Project created by QtCreator 2019-01-14T09:29:24
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = flynote
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    src/flynote.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/flynotetitle.cpp \
    src/flynotebutton.cpp \
    src/colorpicker.cpp \
    src/notelistmodel.cpp

HEADERS += \
    src/flynote.h \
    src/mainwindow.h \
    src/flynotetitle.h \
    src/flynotebutton.h \
    src/colorpicker.h \
    src/notelistmodel.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
