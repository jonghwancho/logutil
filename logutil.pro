TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    source_1.cpp \
    source_2.cpp \
    source_3.cpp

LIBS += -lpthread

HEADERS += \
    source_1.h \
    logutil_main.h \
    logutil.h \
    source_2.h \
    source_3.h
