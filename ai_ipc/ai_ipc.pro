#-------------------------------------------------
#
# Project created by QtCreator 2017-05-29T10:22:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ai_ipc
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/3party/opencv32 \
               $$PWD/3party/opencv32/include/opencv \
               $$PWD/3party/opencv32/include/opencv2

LIBS += -L$$PWD/3party/opencv32 -lopencv_world \
        -L$$PWD/3party/opencv32 -lopencv_contrib_world

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/camera.cpp \
    src/clsdnn.cpp \
    src/clscaffe.cpp

HEADERS  += src/mainwindow.h \
    src/camera.h \
    src/clsdnn.h \
    src/clscaffe.h

FORMS    += src/mainwindow.ui


unix:!macx: LIBS += -L$$PWD/3party/caffe/lib/ -lcaffe

INCLUDEPATH += $$PWD/3party/caffe/include
DEPENDPATH += $$PWD/3party/caffe/include
