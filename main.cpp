#include "gemploye.h"
#include "logindialog.h"

#include <QApplication>
#include <QMessageBox>

#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Connexion à la base de données
    Connection* c = Connection::instance();
    bool test = c->createConnect();

    if (!test)
    {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Database is not open"),
            QObject::tr("Connection failed.\nL'application va se fermer."),
            QMessageBox::Ok
        );
        return 0;
    }

    // Afficher la fenêtre de login (email / mot de passe / Face ID)
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted)
    {
        // Utilisateur a annulé ou échec de connexion
        return 0;
    }

    // Récupérer le rôle de l'utilisateur connecté
    QString userRole = loginDialog.getUserRole();

    // Ouvrir la fenêtre principale de gestion employé/projet
    Gemploye w;
    w.initAfterConnect();
    w.configurePermissions(userRole);
    w.show();

    return a.exec();
}
