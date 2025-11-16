#include "gemploye.h"
#include "logindialog.h"
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
            QObject::tr("Connection successful."),
            QMessageBox::Ok
            );

        LoginDialog login;
        if (login.exec() == QDialog::Accepted) {
            w.show();
        } else {
            return 0; // l'utilisateur a annulé la connexion
        }
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
