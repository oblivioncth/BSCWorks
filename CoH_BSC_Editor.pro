#-------------------------------------------------
#
# Project created by QtCreator 2019-08-05T12:25:36
#
#-------------------------------------------------

QT += core gui

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
    src/mp3.cpp \
    src/smc.cpp \
    src/smf.cpp \
    src/wav.cpp

HEADERS += \
    src/bsc.h \
    src/mainwindow.h \
    src/mp3.h \
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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQx_static32_0-0-2-14_Qt_5-15-0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQx_static32_0-0-2-14_Qt_5-15-0d

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libQx_static32_0-0-2-14_Qt_5-15-0.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libQx_static32_0-0-2-14_Qt_5-15-0d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/Qx_static32_0-0-2-14_Qt_5-15-0.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/Qx_static32_0-0-2-14_Qt_5-15-0d.lib
