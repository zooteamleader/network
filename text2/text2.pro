#-------------------------------------------------
#
# Project created by QtCreator 2021-12-27T18:35:14
#
#-------------------------------------------------

QT       += core gui
QT       += network
RC_ICONS = start.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = text2
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    exp2/mytracert.cpp \
    exp2/myping.cpp \
    exp2/exp2.cpp


HEADERS += \
        mainwindow.h \
    exp2/mytracert.h \
    exp2/myping.h


FORMS += \
        mainwindow.ui

RESOURCES += \
    res.qrc

win32: LIBS += -lws2_32

#CONFIG(release, debug|release){
#    RC_FILE = uac.rc
#}

#RC_FILE = uac.rc



