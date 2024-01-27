QT += core serialbus
CONFIG += c++11
DEFINES += QT_STATICPLUGIN
INCLUDEPATH += $$PWD
QMAKE_CXXFLAGS += -Wall

#CONFIG += plugin

SOURCES += $$PWD/plugin.cpp\
    $$PWD/device.cpp\
    $$PWD/devices.cpp\
    $$PWD/interface.cpp \
    $$PWD/listener.cpp\
    $$PWD/timing.cpp\

HEADERS += \
    $$PWD/device.hpp \
    $$PWD/devices.hpp\
    $$PWD/interface.hpp \
    $$PWD/listener.hpp \
    $$PWD/plugin.hpp \
    $$PWD/timing.hpp\

#CandleLight API
SOURCES += \
    $$PWD/api/candle.c \
    $$PWD/api/candle_ctrl_req.c

HEADERS += \
    $$PWD/api/candle.h \
    $$PWD/api/candle_defs.h \
    $$PWD/api/ch_9.h

win32: LIBS += -lsetupapi
win32: LIBS += -lole32
win32: LIBS += -lwinusb

