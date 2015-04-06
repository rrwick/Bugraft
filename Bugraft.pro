#-------------------------------------------------
#
# Project created by QtCreator 2014-12-12T18:36:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bugraft
TEMPLATE = app


SOURCES += \
    program/main.cpp\
    program/globals.cpp \
    program/settings.cpp \
    program/landscape.cpp \
    program/randomnumbers.cpp \
    program/saverandloader.cpp \
    bug/organism.cpp \
    bug/genome.cpp \
    bug/species.cpp \
    history/historydatapoint.cpp \
    history/history.cpp \
    tree/square/squaretreegraphicsscene.cpp \
    tree/square/squaretreespeciesline.cpp \
    tree/square/squaretreehorizontalline.cpp \
    tree/curved/curvedtreeline.cpp \
    tree/curved/curvedtreegraphicsscene.cpp \
    ui/mainwindow.cpp \
    ui/infotextwidget.cpp \
    ui/environmentgraphicsscene.cpp \
    ui/graphicsviewzoom.cpp \
    ui/landscapeeditdialog.cpp \
    ui/waterlevelvisualaid.cpp \
    ui/organisminfodialog.cpp \
    ui/singleorganismwidget.cpp \
    ui/landscapehistorywidget.cpp \
    ui/settingsdialog.cpp \
    ui/waitingdialog.cpp \
    ui/treegraphicsview.cpp

HEADERS  += \
    program/globals.h \
    program/settings.h \
    program/landscape.h \
    program/randomnumbers.h \
    program/saverandloader.h \
    program/landscapeparameters.h \
    bug/organism.h \
    bug/genome.h \
    bug/species.h \
    history/historydatapoint.h \
    history/history.h \
    tree/square/squaretreegraphicsscene.h \
    tree/square/squaretreespeciesline.h \
    tree/square/squaretreehorizontalline.h \
    tree/curved/curvedtreeline.h \
    tree/curved/curvedtreegraphicsscene.h \
    ui/mainwindow.h \
    ui/infotextwidget.h \
    ui/environmentgraphicsscene.h \
    ui/graphicsviewzoom.h \
    ui/landscapeeditdialog.h \
    ui/waterlevelvisualaid.h \
    ui/organisminfodialog.h \
    ui/singleorganismwidget.h \
    ui/landscapehistorywidget.h \
    ui/settingsdialog.h \
    ui/waitingdialog.h \
    ui/treegraphicsview.h

FORMS    += ui/mainwindow.ui \
    ui/landscapeeditdialog.ui \
    ui/organisminfodialog.ui \
    ui/settingsdialog.ui \
    ui/waitingdialog.ui

RESOURCES += \
    images/images.qrc

INCLUDEPATH += ui

unix:QMAKE_CXXFLAGS += -std=c++11
unix:INCLUDEPATH += /usr/include/
unix:LIBS += -L/usr/lib
unix:LIBS += -lboost_iostreams
unix:LIBS += -lboost_serialization
unix:LIBS += -lnoise

win32:LIBS += -llibnoise
win32:LIBS += -L$$PWD/../libnoise/bin/
win32:INCLUDEPATH += $$PWD/../libnoise/include/
win32:INCLUDEPATH += $$PWD/../boost_1_55_0/
win32:LIBS += -L$$PWD/../boost_1_55_0/lib64-msvc-12.0/
win32:RC_FILE = images/myapp.rc

macx:INCLUDEPATH += /usr/local/include/
macx:LIBS += -L/usr/local/lib
macx:LIBS += -lboost_iostreams
macx:LIBS += -lboost_serialization
macx:ICON = images/application.icns
