QT += core gui widgets autoupdaterwidgets

CONFIG += c++17

SOURCES += \
    configeditor.cpp \
    converterop.cpp \
    fmsconverter.cpp \
    fplconverter.cpp \
    main.cpp \
    mainwindow.cpp \
    plnconverter.cpp

HEADERS += \
    configeditor.h \
    converter.h \
    converterop.h \
    fmsconverter.h \
    fplconverter.h \
    mainwindow.h \
    plnconverter.h

FORMS += \
    mainwindow.ui

RESOURCES += ../res/res.qrc

INCLUDEPATH += ../lib/include

LIBS += -L../lib -lpugixml

VERSION = 1.0

win32
{
    RC_ICONS = ../res/icon.ico
    QMAKE_TARGET_DESCRIPTION = Convert between major flight plan formats.
    QMAKE_TARGET_COPYRIGHT = Copyright 2021, Abdullah Radwan
}
