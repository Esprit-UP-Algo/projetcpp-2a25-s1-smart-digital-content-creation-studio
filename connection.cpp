#include "connection.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>

Connection::Connection()
{
    // Pas besoin de code ici pour l'instant
}

bool Connection::createconnect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("projet");             // Nom du DSN ODBC
    db.setUserName("smart_content");          // Nom d'utilisateur
    db.setPassword("smart_content123");       // Mot de passe

    if (!db.open()) {
        qDebug() << "Erreur DB:" << db.lastError().text();
        return false;
    }

    qDebug() << "Connexion réussie!";
    return true;
}
