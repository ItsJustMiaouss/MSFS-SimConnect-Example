QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    msfs_connect.cpp

HEADERS += \
    mainwindow.h \
    msfs_connect.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += "$$(MSFS_SDK)/SimConnect SDK/include"

LIBS += -L"$$(MSFS_SDK)/SimConnect SDK/lib" -lSimConnect

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
