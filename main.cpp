#include "gemploye.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gemploye w;
    w.show();
    return a.exec();
}
