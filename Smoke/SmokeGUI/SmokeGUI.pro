#-------------------------------------------------
#
# Project created by QtCreator 2016-11-15T14:24:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmokeGUI
TEMPLATE = app

QMAKE_LFLAGS += /NODEFAULTLIB:MSVCRT

SOURCES += main.cpp\
        mainwindow.cpp \
    mainview.cpp \
    fluids.c \
    simulation.cpp \
    utils.cpp

HEADERS  += mainwindow.h \
    mainview.h \
    fluids.h \
    simulation.h \
    utils.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

unix|win32: LIBS += -L$$PWD/../library/ -lFFTW

INCLUDEPATH += $$PWD/../fftw-2.1.3/rfftw \
            += $$PWD/../fftw-2.1.3/fftw \
            += $$PWD/../library \


DEPENDPATH += $$PWD/../fftw-2.1.3/rfftw \
           += $$PWD/../fftw-2.1.3/fftw \
           += $$PWD/../library \


win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../library/FFTW.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../library/libFFTW.a
