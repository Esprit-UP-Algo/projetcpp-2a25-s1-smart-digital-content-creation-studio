#include "gemploye.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gemploye w;

    // Création d'une instance de Connection
    Connection c;

    // Test de la connexion
    bool test = c.createconnect();

    if (test) {
        QMessageBox::information(
            nullptr,
            QObject::tr("Database is open"),
            QObject::tr("Connection successful.\nClick Cancel to exit."),
            QMessageBox::Cancel
            );
        w.show();
    } else {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Database is not open"),
            QObject::tr("Connection failed.\nClick Cancel to exit."),
            QMessageBox::Cancel
            );
    }

    return a.exec();
}
