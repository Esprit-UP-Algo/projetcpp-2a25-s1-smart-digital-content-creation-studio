#include "gemploye.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Connection* c = Connection::instance();
    bool test = c->createConnect();

    if(!test)
    {
        QMessageBox::critical(nullptr, "Database is not open", "Connection failed.\nClick Cancel to exit.", QMessageBox::Cancel);
        return -1;
    }

    Gemploye w;
    w.show();

    return a.exec();
}
