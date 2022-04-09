QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    canvas.cpp \
    fullscreenpreview.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    openingwindow.cpp

HEADERS += \
    canvas.h \
    fullscreenpreview.h \
    mainwindow.h \
    model.h \
    openingwindow.h

FORMS += \
    fullscreenpreview.ui \
    mainwindow.ui \
    openingwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../../Desktop/480px-Happy_smiley_face.png \
    ../../Desktop/plus.png
