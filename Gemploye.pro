QT       += core gui sql charts multimedia multimediawidgets network printsupport serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Active les parties du code protégées par #ifdef HAVE_CHARTS
DEFINES += HAVE_CHARTS

# ========================================
# Configuration OpenCV MinGW
# ========================================
# DÉSACTIVÉ TEMPORAIREMENT - Bibliothèques non disponibles
# Pour une vraie reconnaissance faciale, il faudra installer OpenCV pré-compilé
# Pour l'instant, on utilise le prototype Face ID (qui fonctionne sans OpenCV)

# OPENCV_DIR = C:/Users/LOQ/Downloads/OpenCV-MinGW-Build-OpenCV-4.5.5-x64
# INCLUDEPATH += $$OPENCV_DIR/include
# LIBS += -L$$OPENCV_DIR/x64/mingw/lib \
#         -lopencv_core455 \
#         -lopencv_imgproc455 \
#         -lopencv_highgui455 \
#         -lopencv_imgcodecs455 \
#         -lopencv_videoio455 \
#         -lopencv_objdetect455 \
#         -lopencv_face455

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connection.cpp \
    employe.cpp \
    main.cpp \
    gemploye.cpp \
    logindialog.cpp \
    arduino.cpp \
    projet.cpp \
    qrcodegenerator.cpp \
    qrcodescanner.cpp \
    gprojet.cpp

HEADERS += \
    connection.h \
    employe.h \
    gemploye.h \
    logindialog.h \
    arduino.h \
    projet.h \
    qrcodegenerator.h \
    qrcodescanner.h \
    gprojet.h

FORMS += \
    gemploye.ui \
    logindialog.ui \
    gprojet.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc

QT+=sql network
