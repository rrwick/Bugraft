#-------------------------------------------------
#
# Project created by QtCreator 2014-12-12T18:36:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bugraft
TEMPLATE = app

CONFIG += c++11


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
    ui/treegraphicsview.cpp \
    ui/aboutdialog.cpp \
    libnoise/model/cylinder.cpp \
    libnoise/model/line.cpp \
    libnoise/model/plane.cpp \
    libnoise/model/sphere.cpp \
    libnoise/module/abs.cpp \
    libnoise/module/add.cpp \
    libnoise/module/billow.cpp \
    libnoise/module/blend.cpp \
    libnoise/module/cache.cpp \
    libnoise/module/checkerboard.cpp \
    libnoise/module/clamp.cpp \
    libnoise/module/const.cpp \
    libnoise/module/curve.cpp \
    libnoise/module/cylinders.cpp \
    libnoise/module/displace.cpp \
    libnoise/module/exponent.cpp \
    libnoise/module/invert.cpp \
    libnoise/module/max.cpp \
    libnoise/module/min.cpp \
    libnoise/module/modulebase.cpp \
    libnoise/module/multiply.cpp \
    libnoise/module/perlin.cpp \
    libnoise/module/power.cpp \
    libnoise/module/ridgedmulti.cpp \
    libnoise/module/rotatepoint.cpp \
    libnoise/module/scalebias.cpp \
    libnoise/module/scalepoint.cpp \
    libnoise/module/select.cpp \
    libnoise/module/spheres.cpp \
    libnoise/module/terrace.cpp \
    libnoise/module/translatepoint.cpp \
    libnoise/module/turbulence.cpp \
    libnoise/module/voronoi.cpp \
    libnoise/latlon.cpp \
    libnoise/noisegen.cpp

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
    ui/treegraphicsview.h \
    ui/aboutdialog.h \
    libnoise/model/cylinder.h \
    libnoise/model/line.h \
    libnoise/model/model.h \
    libnoise/model/plane.h \
    libnoise/model/sphere.h \
    libnoise/module/abs.h \
    libnoise/module/add.h \
    libnoise/module/billow.h \
    libnoise/module/blend.h \
    libnoise/module/cache.h \
    libnoise/module/checkerboard.h \
    libnoise/module/clamp.h \
    libnoise/module/const.h \
    libnoise/module/curve.h \
    libnoise/module/cylinders.h \
    libnoise/module/displace.h \
    libnoise/module/exponent.h \
    libnoise/module/invert.h \
    libnoise/module/max.h \
    libnoise/module/min.h \
    libnoise/module/module.h \
    libnoise/module/modulebase.h \
    libnoise/module/multiply.h \
    libnoise/module/perlin.h \
    libnoise/module/power.h \
    libnoise/module/ridgedmulti.h \
    libnoise/module/rotatepoint.h \
    libnoise/module/scalebias.h \
    libnoise/module/scalepoint.h \
    libnoise/module/select.h \
    libnoise/module/spheres.h \
    libnoise/module/terrace.h \
    libnoise/module/translatepoint.h \
    libnoise/module/turbulence.h \
    libnoise/module/voronoi.h \
    libnoise/basictypes.h \
    libnoise/exception.h \
    libnoise/interp.h \
    libnoise/latlon.h \
    libnoise/mathconsts.h \
    libnoise/misc.h \
    libnoise/noise.h

FORMS    += ui/mainwindow.ui \
    ui/landscapeeditdialog.ui \
    ui/organisminfodialog.ui \
    ui/settingsdialog.ui \
    ui/waitingdialog.ui \
    ui/aboutdialog.ui

RESOURCES += \
    images/images.qrc

INCLUDEPATH += ui

unix:QMAKE_CXXFLAGS += -std=c++11
unix:INCLUDEPATH += /usr/include/
unix:LIBS += -L/usr/lib
unix:LIBS += -lboost_iostreams
unix:LIBS += -lboost_serialization

win32:INCLUDEPATH += $$PWD/../boost_1_55_0/
win32:LIBS += -L$$PWD/../boost_1_55_0/lib64-msvc-12.0/
win32:RC_FILE = images/myapp.rc

macx:INCLUDEPATH += /usr/local/include/
macx:LIBS += -L/usr/local/lib
macx:LIBS += -lboost_iostreams
macx:LIBS += -lboost_serialization
macx:ICON = images/application.icns
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
