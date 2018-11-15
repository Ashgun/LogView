TARGET = LogView

greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets

CONFIG += c++14
CONFIG += console
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

INCLUDEPATH += ../3rdparty/include
LIBS += ../3rdparty/lib/libjsoncpp.a

SOURCES += src/main.cpp \
    src/RegExpLogLineParser.cpp \
    src/RegExpsLogMessageParser.cpp \
    src/LinePosition.cpp \
    src/BaseLinePositionStorage.cpp \
    src/FilesIndexer.cpp \
    src/EventPattern.cpp \
    src/LineMatcher.cpp \
    src/BasePositionedLinesStorage.cpp \
    src/Events.cpp \
    src/PositionedLine.cpp \
    src/LogViewMainWindow.cpp \
    src/CustomView.cpp \
    src/CustomItem.cpp \
    src/EventGraphicsItem.cpp \
    src/EventsGraphicsScene.cpp \
    src/LogLineHeaderParsingParamsEditWidget.cpp \
    src/ColorSelectionViewWidget.cpp \
    src/LinePatternEditWidget.cpp \
    src/SingleLinePatternEditWidget.cpp \
    src/EventPatternEditWidget.cpp \
    src/ExtendLinePatternEditWidget.cpp \
    src/EventPatternsEditDialog.cpp \
    src/EventsTreeWidget.cpp \
    src/RecentFilesWidget.cpp \
    src/LogFileWithConfigsOpenDialog.cpp \
    src/Utils.cpp \
    src/EventsTreeEditWidget.cpp

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
    src/BasePositionedLinesStorage.h \
    src/Events.h \
    src/PositionedLine.h \
    src/LogViewMainWindow.h \
    src/CustomView.h \
    src/CustomItem.h \
    src/EventGraphicsItem.h \
    src/EventsGraphicsScene.h \
    src/LogLineHeaderParsingParamsEditWidget.h \
    src/ColorSelectionViewWidget.h \
    src/LinePatternEditWidget.h \
    src/SingleLinePatternEditWidget.h \
    src/EventPatternEditWidget.h \
    src/ExtendLinePatternEditWidget.h \
    src/EventPatternsEditDialog.h \
    src/EventsTreeWidget.h \
    src/RecentFilesWidget.h \
    src/Common.h \
    src/LogFileWithConfigsOpenDialog.h \
    src/Utils.h \
    src/EventsTreeEditWidget.h
