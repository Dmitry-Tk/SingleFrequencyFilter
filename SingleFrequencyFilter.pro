#-------------------------------------------------
#
# Project created by QtCreator 2021-06-04T10:06:54
#
#-------------------------------------------------

QT += core gui charts widgets

TARGET = SingleFrequencyFilter
TEMPLATE = app


SOURCES += SingleFrequencyFilter.cpp \
    ../../Arxiv/cfourierfilter.cpp \
    cfourierfilt.cpp \
    csignal.cpp \
    cwidget.cpp \
    qtplotter.cpp

HEADERS  += \
    ../../Arxiv/cfourierfilter.h \
    cfourierfilt.h \
    csignal.h \
    cwidget.h \
    qtplotter.h

QMAKE_LFLAGS += -no-pie
