QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    robo_trace.cpp

HEADERS += \
    lib/zaux.h \
    lib/zmotion.h \
    mainwindow.h \
    robo_trace.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/lib/ -lzaux


INCLUDEPATH += $$PWD/lib
DEPENDPATH += $$PWD/lib


QXLSX_PARENTPATH=$$PWD/QXlsx/         # ./当前工程目录路径 指定QXlsx
QXLSX_HEADERPATH=$$PWD/QXlsx/header/  # ./当前工程目录路径 指定header文件
QXLSX_SOURCEPATH=$$PWD/QXlsx/source/  # ./当前工程目录路径 指定source文件
include($$PWD/QXlsx/QXlsx.pri)

# include($$PWD/QXlsx/QXlsx.pri)             # QXlsx源代码
# INCLUDEPATH += $$PWD/QXlsx
