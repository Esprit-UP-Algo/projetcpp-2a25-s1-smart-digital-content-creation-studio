QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += sql network printsupport

# Try to add charts module if available
qtHaveModule(charts) {
    QT += charts
    DEFINES += HAVE_CHARTS
}

# Try to add serialport module if available
qtHaveModule(serialport) {
    QT += serialport
    DEFINES += HAVE_SERIALPORT
}

# Try to add multimedia module for QR code support
qtHaveModule(multimedia) {
    QT += multimedia
    DEFINES += HAVE_MULTIMEDIA
}

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    arduino.cpp \
    connection.cpp \
    main.cpp \
    gemploye.cpp \
    projet.cpp \
    qrcodegenerator.cpp \
    qrcodescanner.cpp

HEADERS += \
    arduino.h \
    connection.h \
    gemploye.h \
    projet.h \
    qrcodegenerator.h \
    qrcodescanner.h

FORMS += \
    gemploye.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
    