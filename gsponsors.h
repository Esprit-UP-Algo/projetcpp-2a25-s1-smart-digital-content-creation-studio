#ifndef GSPONSORS_H
#define GSPONSORS_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Gsponsors;
}
QT_END_NAMESPACE

class Gsponsors : public QMainWindow
{
    Q_OBJECT

public:
    Gsponsors(QWidget *parent = nullptr);
    ~Gsponsors();

private:
    Ui::Gsponsors *ui;
};
#endif // GSPONSORS_H
