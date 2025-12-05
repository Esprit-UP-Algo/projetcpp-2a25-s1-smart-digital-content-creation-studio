#ifndef GEMPLOYE_H
#define GEMPLOYE_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidget>

#include "materiel.h"
#include "employe.h"
#include "createur.h"

#include <QtCharts/QChartView>

QT_BEGIN_NAMESPACE
namespace Ui { class Gemploye; }
QT_END_NAMESPACE

class Gemploye : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gemploye(QWidget *parent = nullptr);
    ~Gemploye();

private slots:
    // Navigation entre les pages
    void on_Employ_clicked();
    void on_Employ_4_clicked();
    void on_Employ_2_clicked();
    void on_Employ_6_clicked();
    void on_Employ_3_clicked();
    void on_Employ_5_clicked();

    // Employe
    void on_pushButton_clicked();                    // ajouter employé
    void on_tableWidget_cellClicked(int row, int column);
    void on_pushButton_20_clicked();                 // modifier employé
    void on_pushButton_2_clicked();                  // supprimer employé

    // Materiel
    void on_pushButton_8_clicked();                  // ajouter matériel
    void on_tableWidget_6_cellClicked(int row, int column);
    void on_pushButton_11_clicked();                 // modifier matériel
    void on_pushButton_9_clicked();                  // supprimer matériel

    // Createur
    void on_pushButton_19_clicked();                 // ajouter / modifier créateur
    void on_pushButton_25_clicked();                 // supprimer créateur
    void on_pushButton_22_clicked();                 // générer PDF affectation + ajouter à l'historique
    void on_pushButton_24_clicked();                 // générer PDF de l'historique (tableWidget_7)
    void on_pushButton_26_clicked();                 // paiement Stripe

    void on_tableWidget_5_cellClicked(int row, int column);  // sélection créateur
    void on_lineEdit_31_textChanged(const QString &text);    // recherche
    void on_comboBox_6_currentIndexChanged(int index);       // tri

private:
    Ui::Gemploye *ui;

    // Materiel
    Materiel Mtmp;
    QString selectedReference;

    // Employe
    QString selectedCin;

    // Createur
    int selectedCreateurId;
    QChartView *createurChartView;

    // Helpers Createur
    void clearCreateurForm();
    void refreshCreateurTable();
    void updateCreateurStats();
};

#endif // GEMPLOYE_H
