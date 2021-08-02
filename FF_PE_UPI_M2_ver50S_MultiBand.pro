r#-------------------------------------------------
#
# Project created by QtCreator 2016-07-11T12:11:57
#
#-------------------------------------------------

QT       += \
            core gui \
            serialport\
            network widgets\
            winextras

#DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT

DEFINES += SAMPLESPERPOINT=1024\ #keep samples point in power of 2 to keep the DMA notification aligned
        += BURSTSIZE=16\
        += BYTESPERSAMPLE=2\
        += NUMOFBANDS=3\
        += ACTUALSYSTEM\
        += SPECTBASESIZE=400\
        += MAXVTWAMRANGES=10

#+= ACTUALSYSTEM
#+= DAQ_DEBUG_LOGS\
#+= SIG_FIFO_LEN=512\
#+= VERTCAL_RANGE=3.49\

CONFIG  += qwt

LIBS += vfw32.lib Gdi32.lib User32.lib $$PWD/Spectrum/c_header/spcm_win64_msvcpp.lib

#Spectrum Digitizer
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Spectrum/c_header/ -lspcm_win64_msvcpp
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Spectrum/c_header/ -lspcm_win64_msvcpp

INCLUDEPATH += $$PWD/Spectrum/c_header\
            += $$PWD/Spectrum/common \
            += $$PWD/Spectrum/sb5_file

DEPENDPATH += $$PWD/c_header

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FF_PE_UPI_M2_MultiBand
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    laserController.cpp \
    ldvController.cpp \
    Spectrum/common/spcm_lib_card.cpp \
    Spectrum/common/spcm_lib_data.cpp \
    Spectrum/common/ostools/spcm_ostools_win.cpp \
    daqControllerS.cpp \
    dataProcessor.cpp \
    plot.cpp \
    spectrogram.cpp \
    stageController.cpp \
    dialogenlarge.cpp \
    bandpasscontroller.cpp


HEADERS  += mainwindow.h \
    laserController.h \
    structDef.h \
    ldvController.h \
    daqControllerS.h \
    dataProcessor.h \
    plot.h \
    spectrogram.h \
    stageController.h \
    dialogenlarge.h \
    bandpasscontroller.h

FORMS    += mainwindow.ui \
    dialogenlarge.ui

RESOURCES += \
    MyRes.qrc
