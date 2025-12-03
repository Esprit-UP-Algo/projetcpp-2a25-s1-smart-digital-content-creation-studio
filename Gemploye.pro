QT += core gui widgets sql charts texttospeech

CONFIG += c++17 debug

SOURCES += \
    connection.cpp \
    gemploye.cpp \
    gestion_sponsor.cpp \
    main.cpp

HEADERS += \
    connection.h \
    gemploye.h \
    gestion_sponsor.h

FORMS += \
    gemploye.ui

RESOURCES += \
    img.qrc

TARGET = Gemploye
TEMPLATE = app
