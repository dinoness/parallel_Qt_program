QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    app/AppContext.cpp \
    main.cpp \
    mainwindow.cpp \
    motion/TrajectoryFile.cpp \
    motion/TrajectoryGenerator.cpp \
    protocol/CartJogProtocol.cpp \
    protocol/CommandWriter.cpp \
    protocol/ControllerProtocol.cpp \
    protocol/JointProtocol.cpp \
    protocol/TableBufferWriter.cpp \
    protocol/TraceProtocol.cpp \
    service/ConnectionService.cpp \
    service/MotionService.cpp \
    service/TrajectoryService.cpp \
    worker/TrajectorySendWorker.cpp \
    zmotion/ZMotionDriver.cpp

HEADERS += \
    app/AppContext.h \
    core/ProtocolConstants.h \
    core/Result.h \
    mainwindow.h \
    motion/TrajectoryTypes.h \
    motion/TrajectoryFile.h \
    motion/TrajectoryGenerator.h \
    protocol/CartJogProtocol.h \
    protocol/CommandWriter.h \
    protocol/ControllerProtocol.h \
    protocol/JointProtocol.h \
    protocol/TableBufferWriter.h \
    protocol/TraceProtocol.h \
    service/ConnectionService.h \
    service/MotionService.h \
    service/TrajectoryService.h \
    third_party/zaux/zaux.h \
    third_party/zaux/zmotion.h \
    worker/TrajectorySendWorker.h \
    zmotion/ZMotionDriver.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/third_party/zaux -lzaux


INCLUDEPATH += $$PWD/third_party/zaux
DEPENDPATH += $$PWD/third_party/zaux


QXLSX_PARENTPATH=$$PWD/QXlsx/         # ./当前工程目录路径 指定QXlsx
QXLSX_HEADERPATH=$$PWD/QXlsx/header/  # ./当前工程目录路径 指定header文件
QXLSX_SOURCEPATH=$$PWD/QXlsx/source/  # ./当前工程目录路径 指定source文件
include($$PWD/QXlsx/QXlsx.pri)

# include($$PWD/QXlsx/QXlsx.pri)             # QXlsx源代码
# INCLUDEPATH += $$PWD/QXlsx
