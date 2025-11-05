#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QString>

class Connection
{
public:
    Connection();           // Constructeur
    bool createconnect();   // Méthode pour créer la connexion
};

#endif // CONNECTION_H
