QT       += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    connection.cpp \
    employe.cpp \
    main.cpp \
    gemploye.cpp \
    materiel.cpp \
    createur.cpp

HEADERS += \
    connection.h \
    employe.h \
    gemploye.h \
    materiel.h \
    createur.h

FORMS += \
    gemploye.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
