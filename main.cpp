#include <QApplication>
#include "gemploye.h"
#include "connection.h"
#include <QMessageBox>

// ⚠️ IMPORTANT: int main(int argc, char *argv[])
// ⚠️ PAS: int qMain(int argc, char *argv[])
// ⚠️ PAS: int qmain(int argc, char *argv[])

int qMain(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Création de la connexion à la base
    Connection* c = Connection::instance();
    bool test = c->createConnect();

    if(!test)
    {
        QMessageBox::critical(nullptr, "Database Error", "Connection failed!", QMessageBox::Cancel);
        return -1;
    }

    Gemploye window;
    window.show();

    return app.exec();
}
