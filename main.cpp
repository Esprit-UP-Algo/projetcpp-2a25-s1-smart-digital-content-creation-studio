#include "gsponsors.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gsponsors w;
    w.show();
    return a.exec();
}
