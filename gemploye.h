#ifndef GEMPLOYE_H
#define GEMPLOYE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Gemploye;
}
QT_END_NAMESPACE

class Gemploye : public QMainWindow
{
    Q_OBJECT

public:
    Gemploye(QWidget *parent = nullptr);
    ~Gemploye();

private slots:
    void on_Employ_clicked();

    void on_Employ_4_clicked();

    void on_Employ_2_clicked();

    void on_Employ_6_clicked();

    void on_Employ_3_clicked();

    void on_Employ_5_clicked();

private:
    Ui::Gemploye *ui;
};
#endif // GEMPLOYE_H
