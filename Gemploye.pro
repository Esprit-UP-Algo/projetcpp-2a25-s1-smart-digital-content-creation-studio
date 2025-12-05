# Modules Qt utilisés
QT       += core gui widgets sql printsupport network charts
CONFIG   += c++17

SOURCES += main.cpp \
           createur.cpp \
           employee.cpp \
           gemploye.cpp \
           materiel.cpp \
           connection.cpp   # <- add this

HEADERS += createur.h \
           employee.h \
           gemploye.h \
           materiel.h \
           connection.h     # <- add this

FORMS += gemploye.ui


RESOURCES += \
    img.qrc
