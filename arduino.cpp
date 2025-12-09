#include "arduino.h"
#include "employe.h"
#ifdef HAVE_SERIALPORT
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#endif
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QObject>
#include <QDebug>
#include <QDateTime>

Arduino::Arduino() : QObject(nullptr)
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
        serial->setPortName(arduino_port_name);
        if (serial->open(QSerialPort::ReadWrite)){
            serial->setBaudRate(QSerialPort::Baud9600);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
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
        serial->write(d);
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

// ========== NOUVELLES MÉTHODES POUR POINTAGE ==========

void Arduino::handleArduinoData()
{
#ifdef HAVE_SERIALPORT
    if (!serial || !serial->isOpen()) {
        return;
    }

    QByteArray newData = serial->readAll();
    serialbuffer.append(QString::fromUtf8(newData));

    while (serialbuffer.contains('\n')) {
        int pos = serialbuffer.indexOf('\n');
        QString line = serialbuffer.left(pos).trimmed();
        serialbuffer.remove(0, pos + 1);

        qDebug() << "[ARDUINO] Reçu:" << line;

        if (line.startsWith("CIN:")) {
            QString cinStr = line.mid(4);
            bool ok;
            int cin = cinStr.toInt(&ok);

            if (ok) {
                qDebug() << "[ARDUINO] CIN détecté:" << cin;
                verifierEmploye(cin);
            } else {
                qDebug() << "[ARDUINO] CIN invalide (format):" << cinStr;
                envoyerReponse("CIN_INVALIDE");
            }
        }
        else if (line.startsWith("POINTAGE:")) {
            // Format: "POINTAGE:CIN:TYPE"
            QStringList parts = line.split(':');
            if (parts.size() == 3) {
                bool ok;
                int cin = parts[1].toInt(&ok);
                QString type = parts[2];

                if (ok && (type == "IN" || type == "OUT")) {
                    qDebug() << "[ARDUINO] Pointage:" << cin << ":" << type;
                    gererPointage(cin, type);
                } else {
                    qDebug() << "[ARDUINO] Pointage invalide:" << line;
                    envoyerReponse("POINTAGE_ERROR");
                }
            }
        }
        else if (line == "ARDUINO_READY") {
            qDebug() << "[ARDUINO] Arduino prêt!";
        }
    }
#endif
}

void Arduino::verifierEmploye(int cin)
{
    qDebug() << "[DB] Recherche du CIN:" << cin;

    QString role = employee::getRoleByCin(cin);

    if (!role.isEmpty()) {
        qDebug() << "[DB] Employé trouvé - Role:" << role;
        envoyerRole(role);
    } else {
        qDebug() << "[DB] CIN inconnu";
        envoyerReponse("CIN_INVALIDE");
    }
}

void Arduino::gererPointage(int cin, const QString &type)
{
    qDebug() << "[POINTAGE] Gestion du pointage:" << cin << ":" << type;

    if (type == "IN") {
        if (estDejaPointeIN(cin)) {
            qDebug() << "[POINTAGE] Double IN détecté";
            envoyerReponse("DOUBLE_IN");
            return;
        }
    } else if (type == "OUT") {
        if (!existePointageINValide(cin)) {
            qDebug() << "[POINTAGE] OUT sans IN préalable";
            envoyerReponse("NO_IN_FOUND");
            return;
        }
    }

    if (enregistrerPointage(cin, type)) {
        qDebug() << "[POINTAGE] Pointage enregistré avec succès";
        envoyerReponse("POINTAGE_OK");
    } else {
        qDebug() << "[POINTAGE] Erreur lors de l'enregistrement";
        envoyerReponse("POINTAGE_ERROR");
    }
}

void Arduino::envoyerRole(const QString &role)
{
#ifdef HAVE_SERIALPORT
    if (!serial || !serial->isWritable()) {
        qDebug() << "[ARDUINO] Impossible d'écrire sur le port série";
        return;
    }

    QString message = "ROLE:" + role + "\n";
    serial->write(message.toUtf8());
    serial->flush();

    qDebug() << "[ARDUINO] Envoyé:" << message.trimmed();
#endif
}

void Arduino::envoyerReponse(const QString &reponse)
{
#ifdef HAVE_SERIALPORT
    if (!serial || !serial->isWritable()) {
        qDebug() << "[ARDUINO] Impossible d'écrire sur le port série";
        return;
    }

    QString message = reponse + "\n";
    serial->write(message.toUtf8());
    serial->flush();

    qDebug() << "[ARDUINO] Envoyé:" << message.trimmed();
#endif
}

bool Arduino::enregistrerPointage(int cin, const QString &type)
{
    QSqlQuery query;
    QDateTime now = QDateTime::currentDateTime();
    QString dateStr = now.toString("yyyy-MM-dd");
    QString heureStr = now.toString("HH:mm:ss");

    query.prepare("INSERT INTO POINTAGE (CIN, DATE_POINTAGE, HEURE_POINTAGE, TYPE_POINTAGE) "
                  "VALUES (:cin, :date, :heure, :type)");
    query.bindValue(":cin", cin);
    query.bindValue(":date", dateStr);
    query.bindValue(":heure", heureStr);
    query.bindValue(":type", type);

    if (query.exec()) {
        qDebug() << "[DB] Pointage enregistré:" << cin << type << dateStr << heureStr;
        return true;
    } else {
        qDebug() << "[DB] Erreur insertion pointage:" << query.lastError().text();
        return false;
    }
}

bool Arduino::estDejaPointeIN(int cin)
{
    QSqlQuery query;
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    query.prepare("SELECT COUNT(*) FROM POINTAGE WHERE CIN = :cin AND DATE_POINTAGE = :date "
                  "AND TYPE_POINTAGE = 'IN' AND NOT EXISTS "
                  "(SELECT 1 FROM POINTAGE p2 WHERE p2.CIN = :cin AND p2.DATE_POINTAGE = :date "
                  "AND p2.TYPE_POINTAGE = 'OUT' AND p2.HEURE_POINTAGE > POINTAGE.HEURE_POINTAGE)");
    query.bindValue(":cin", cin);
    query.bindValue(":date", dateStr);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool Arduino::existePointageINValide(int cin)
{
    QSqlQuery query;
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    query.prepare("SELECT COUNT(*) FROM POINTAGE WHERE CIN = :cin AND DATE_POINTAGE = :date "
                  "AND TYPE_POINTAGE = 'IN' AND NOT EXISTS "
                  "(SELECT 1 FROM POINTAGE p2 WHERE p2.CIN = :cin AND p2.DATE_POINTAGE = :date "
                  "AND p2.TYPE_POINTAGE = 'OUT' AND p2.HEURE_POINTAGE > POINTAGE.HEURE_POINTAGE)");
    query.bindValue(":cin", cin);
    query.bindValue(":date", dateStr);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
