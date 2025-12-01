#include "arduino.h"
#ifdef HAVE_SERIALPORT
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#endif
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QObject>
#include <QDebug>
Arduino::Arduino()
{
    data="";
    arduino_port_name="";
    arduino_is_available=false;
#ifdef HAVE_SERIALPORT
    serial=new QSerialPort;
#else
    serial=nullptr;
#endif
}

QString Arduino::getarduino_port_name()
{
    return arduino_port_name;
}

#ifdef HAVE_SERIALPORT
QSerialPort *Arduino::getserial()
{
    return serial;
}
#endif
int Arduino::connect_arduino()
{
#ifdef HAVE_SERIALPORT
    // Recherche du port sur lequel la carte arduino identifée par arduino_uno_vendor_id
    // est connectée
    serialbuffer = "";
    foreach (const QSerialPortInfo &serial_port_info, QSerialPortInfo::availablePorts()){
        qDebug() << "Port:" << serial_port_info.portName();
        qDebug() << "Vendor:" << serial_port_info.vendorIdentifier();
        qDebug() << "Product:" << serial_port_info.productIdentifier();

        if (serial_port_info.hasVendorIdentifier() && serial_port_info.hasProductIdentifier()){
            if ((serial_port_info.vendorIdentifier() == arduino_uno_vendor_id && serial_port_info.productIdentifier() == arduino_uno_producy_id) ||
                (serial_port_info.vendorIdentifier() == ch340_vendor_id && serial_port_info.productIdentifier() == ch340_product_id)) {
                arduino_is_available = true;
                arduino_port_name = serial_port_info.portName();
                qDebug() << "Arduino found on port:" << arduino_port_name;
                break;
            }
        }
    }

    if (arduino_port_name.isEmpty()) {
        qDebug() << "Arduino non disponible.";
        return -1;
    }

    qDebug() << "Arduino port name is: " << arduino_port_name;

    if (arduino_is_available){
        // Configuration de la communication (débit...)
        serial->setPortName(arduino_port_name);
        if (serial->open(QSerialPort::ReadWrite)){
            serial->setBaudRate(QSerialPort::Baud9600); // débit : 9600 bits/s
            serial->setDataBits(QSerialPort::Data8); // Longueur des données : 8 bits,
            serial->setParity(QSerialPort::NoParity); // 1 bit de parité optionnel
            serial->setStopBits(QSerialPort::OneStop); // Nombre de bits de stop : 1
            serial->setFlowControl(QSerialPort::NoFlowControl);
            return 0;
        }
        return 1;
    }
#else
    qDebug() << "SerialPort module not available. Arduino support disabled.";
#endif
    return -1;
}


int Arduino::close_arduino()
{
#ifdef HAVE_SERIALPORT
    if(serial->isOpen()){
        serial->close();
        return 0;
    }
#endif
    return 1;
}


QByteArray Arduino::read_from_arduino()
{
#ifdef HAVE_SERIALPORT
    if(serial->isReadable()){
        serial->waitForReadyRead(10);
        data=serial->readAll();
        return data;
    }
#endif
    return QByteArray();
}


QByteArray Arduino::getdata()
{
    return data;
}
int Arduino::write_to_arduino(QByteArray d)
{
#ifdef HAVE_SERIALPORT
    if(serial->isWritable()){
        serial->write(d);  // envoyer des données vers Arduino
        return 0;
    }else{
        qDebug() << "Couldn't write to serial!";
        return -1;
    }
#else
    qDebug() << "SerialPort module not available. Cannot write to Arduino.";
    return -1;
#endif
}
