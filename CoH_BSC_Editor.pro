#-------------------------------------------------
#
# Project created by QtCreator 2019-08-05T12:25:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BSCWorks
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17
QMAKE_CXXFLAGS += /std:c++17

SOURCES += \
    src/bsc.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/smc.cpp \
    src/smf.cpp \
    src/wav.cpp

HEADERS += \
    resources.rc \
    src/bsc.h \
    src/mainwindow.h \
    src/smc.h \
    src/smf.h \
    src/version.h \
    src/wav.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RC_FILE = resources.rc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lqxtended_static_0_0_1_0_Qt_5_12_3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lqxtended_static_0_0_1_0_Qt_5_12_3d

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libqxtended_static_0_0_1_0_Qt_5_12_3.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libqxtended_static_0_0_1_0_Qt_5_12_3d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/qxtended_static_0_0_1_0_Qt_5_12_3.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/qxtended_static_0_0_1_0_Qt_5_12_3d.lib
