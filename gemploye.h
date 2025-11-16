#ifndef GEMPLOYE_H
#define GEMPLOYE_H
#include "employe.h"

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


    void on_ajouter_clicked();

    void on_refrech_clicked();


    void on_modifier_clicked();

    void on_tableemp_clicked(const QModelIndex &index);

    void on_supprimer_clicked();

    void on_lineEdit_13_textChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_5_clicked();

    void on_actualiser_clicked();

    void on_tabWidget_currentChanged(int index);

private:
    Ui::Gemploye *ui;
    employee e_global;
    int selectedId = 0;  // instance globale pour CRUD
    bool validerChamps(QString &erreur);

    void afficherStatistiques();

};
#endif // GEMPLOYE_H
