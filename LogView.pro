TARGET = LogView

QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += src/main.cpp \
    src/RegExpLogLineParser.cpp \
    src/RegExpsLogMessageParser.cpp \
    src/LinePosition.cpp \
    src/BaseLinePositionStorage.cpp \
    src/FilesIndexer.cpp \
    src/EventPattern.cpp \
    src/LineMatcher.cpp \
    src/BasePositionedLinesStorage.cpp

HEADERS += \
    src/ILogLineParser.h \
    src/LogLineInfo.h \
    src/RegExpLogLineParser.h \
    src/ILogMessageParser.h \
    src/RegExpsLogMessageParser.h \
    src/LinePosition.h \
    src/ILinePositionStorage.h \
    src/BaseLinePositionStorage.h \
    src/FilesIndexer.h \
    src/EventPattern.h \
    src/LineMatcher.h \
    src/ILineSelector.h \
    src/IPositionedLinesStorage.h \
    src/BasePositionedLinesStorage.h
