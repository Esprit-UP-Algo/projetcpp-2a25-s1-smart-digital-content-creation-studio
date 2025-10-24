#include "gemploye.h"

#include <QApplication>

#include <QMessageBox>
#include "connection.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gemploye w;

    // Retrieve the unique instance of the Connection class
    Connection* c = Connection::instance();

    // Test the database connection
    bool test = c->createConnect();

    if (test) {
        w.show();
        QMessageBox::information(
            nullptr,
            QObject::tr("Database is open"),
            QObject::tr("Connection successful.\nClick Cancel to exit."),
            QMessageBox::Cancel
            );
    } else {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Database is not open"),
            QObject::tr("Connection failed.\nClick Cancel to exit."),
            QMessageBox::Cancel
            );
    }
    w.show();

    return a.exec();
}



/*int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gemploye w;
    w.show();
    return a.exec();
}*/
