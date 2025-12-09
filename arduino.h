#ifndef ARDUINO_H
#define ARDUINO_H

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

#include "employe.h"

#ifdef HAVE_SERIALPORT
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#endif

class Arduino : public QObject
{
    Q_OBJECT

public:
    Arduino();
    int connect_arduino();
    int close_arduino();
    QByteArray read_from_arduino();
    int write_to_arduino(QByteArray d);
    QString getarduino_port_name();
    QByteArray getdata();

#ifdef HAVE_SERIALPORT
    QSerialPort *getserial();
#endif

public slots:
    void handleArduinoData();

private:
    // Gestion des données et de la BD
    void verifierEmploye(int cin);
    void gererPointage(int cin, const QString &type);
    void envoyerRole(const QString &role);
    void envoyerReponse(const QString &reponse);
    bool enregistrerPointage(int cin, const QString &type);
    bool estDejaPointeIN(int cin);
    bool existePointageINValide(int cin);

    QByteArray data;
    QString arduino_port_name;
    bool arduino_is_available;
    QString serialbuffer;

#ifdef HAVE_SERIALPORT
    QSerialPort *serial;
#else
    void *serial;
#endif

    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_producy_id = 67;
    static const quint16 ch340_vendor_id = 0x1a86;
    static const quint16 ch340_product_id = 0x7523;
};

#endif // ARDUINO_H
