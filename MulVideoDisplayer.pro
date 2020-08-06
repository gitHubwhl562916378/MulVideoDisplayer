#-------------------------------------------------
#
# Project created by QtCreator 2020-07-26T10:43:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets concurrent

TARGET = MulVideoDisplayer
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
include(Service/service.pri)
include(Dao/dao.pri)
include(VideoWidget/videowidget.pri)
SOURCES += \
        errorlabel.cpp \
        main.cpp \
        mainwindow.cpp \
        player.cpp \
        realplaymanager.cpp \
        waitinglabel.cpp \
        windowtitlebar.cpp

HEADERS += \
        common_data.h \
        errorlabel.h \
        mainwindow.h \
        player.h \
        realplaymanager.h \
        waitinglabel.h \
        widgetinterface.h \
        windowtitlebar.h

INCLUDEPATH += E:/ffmpeg/x64/include \
               $$(INTELMEDIASDKROOT)/include \
               $$PWD/3rd/curl/include

LIBS += -LE:/ffmpeg/x64/lib \
        $$PWD/3rd/curl/lib/windows/win64/libcurl-d_imp.lib \
        avcodec.lib avdevice.lib avfilter.lib avformat.lib avutil.lib postproc.lib swresample.lib swscale.lib \
        ws2_32.lib
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
