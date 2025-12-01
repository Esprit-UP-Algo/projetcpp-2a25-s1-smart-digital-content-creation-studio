#ifndef ARDUINO_H
#define ARDUINO_H

#ifdef HAVE_SERIALPORT
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#else
#include <QObject>
#endif
#include <QDebug>

class Arduino
{
public:
    Arduino();
    int connect_arduino();
    int close_arduino();
    int write_to_arduino(QByteArray d);
    QByteArray read_from_arduino();
#ifdef HAVE_SERIALPORT
    QSerialPort *getserial();
#endif
    QString getarduino_port_name();
    QByteArray getdata();

private:
#ifdef HAVE_SERIALPORT
    QSerialPort *serial;
#else
    void *serial;
#endif
    static const quint16 arduino_uno_vendor_id = 0x2341;
    static const quint16 arduino_uno_producy_id = 0x0043;
    static const quint16 ch340_vendor_id = 0x1a86;
    static const quint16 ch340_product_id = 0x7523;
    QString arduino_port_name;
    bool arduino_is_available;
    QByteArray data;
    QString serialbuffer;
};

#endif // ARDUINO_H
