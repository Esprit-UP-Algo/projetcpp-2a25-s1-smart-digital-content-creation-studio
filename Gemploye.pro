<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
QT       += core gui sql
=======
QT       += core gui sql charts multimedia multimediawidgets network printsupport serialport texttospeech
>>>>>>> Stashed changes
=======
QT       += core gui sql charts multimedia multimediawidgets network printsupport serialport texttospeech
>>>>>>> Stashed changes
=======
QT       += core gui sql charts multimedia multimediawidgets network printsupport serialport texttospeech
>>>>>>> Stashed changes
=======
QT       += core gui sql charts multimedia multimediawidgets network printsupport serialport texttospeech
>>>>>>> Stashed changes
=======
QT       += core gui sql charts multimedia multimediawidgets network printsupport serialport texttospeech
>>>>>>> Stashed changes

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
# Active les parties du code protégées par #ifdef HAVE_CHARTS et #ifdef HAVE_SERIALPORT
DEFINES += HAVE_CHARTS
DEFINES += HAVE_SERIALPORT

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

>>>>>>> Stashed changes
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connection.cpp \
    employe.cpp \
    main.cpp \
<<<<<<< Updated upstream
    gemploye.cpp
=======
    gemploye.cpp \
    logindialog.cpp \
    arduino.cpp \
    projet.cpp \
    qrcodegenerator.cpp \
    qrcodescanner.cpp \
    contrat.cpp \
    gsponsor.cpp \
    gestion_sponsor.cpp \
    gcontrat.cpp \
    gprojet.cpp
>>>>>>> Stashed changes

HEADERS += \
    connection.h \
    employe.h \
<<<<<<< Updated upstream
    gemploye.h

FORMS += \
    gemploye.ui
=======
    gemploye.h \
    logindialog.h \
    arduino.h \
    projet.h \
    qrcodegenerator.h \
    contrat.h \
    gsponsor.h \
    gestion_sponsor.h \
    gcontrat.h \
    qrcodescanner.h \
    gprojet.h

FORMS += \
    gemploye.ui \
    logindialog.ui \
    gcontrat.ui \
    gsponsor.ui \
    gprojet.ui
>>>>>>> Stashed changes

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc

<<<<<<< Updated upstream
QT+=sql
=======
QT+=sql network

QT += charts

QT += texttospeech


<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
