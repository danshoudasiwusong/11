QT       += core gui widgets multimedia multimediawidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = FaceRecognitionLogin
TEMPLATE = app

SOURCES += \
    main.cpp \
    appcontroller.cpp \
    logindialog.cpp \
    registerdialog.cpp \
    mainwindow.cpp \
    facedetector.cpp \
    facerecognizer.cpp \
    databasemanager.cpp

HEADERS += \
    appcontroller.h \
    logindialog.h \
    registerdialog.h \
    mainwindow.h \
    facedetector.h \
    facerecognizer.h \
    databasemanager.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target