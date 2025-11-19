#ifndef GEMPLOYE_H
#define GEMPLOYE_H

#include <QMainWindow>
#include <QMessageBox>
#include "materiel.h"
#include "employe.h"
#include "createur.h"
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

QT_BEGIN_NAMESPACE
namespace Ui { class Gemploye; }
QT_END_NAMESPACE

class Gemploye : public QMainWindow
{
    Q_OBJECT
public:
    Gemploye(QWidget *parent = nullptr);
    ~Gemploye();

private slots:
    // Navigation
    void on_Employ_clicked();
    void on_Employ_4_clicked();
    void on_Employ_2_clicked();
    void on_Employ_6_clicked();
    void on_Employ_3_clicked();
    void on_Employ_5_clicked();

    // Employe
    void on_pushButton_clicked();                 // Ajouter
    void on_tableWidget_cellClicked(int row, int column);
    void on_pushButton_20_clicked();              // Modifier
    void on_pushButton_2_clicked();               // Supprimer

    // Materiel
    void on_pushButton_8_clicked();   // Ajouter
    void on_pushButton_11_clicked();  // Modifier
    void on_pushButton_9_clicked();   // Supprimer
    void on_tableWidget_6_cellClicked(int row, int column);

    // Createur
    void on_pushButton_19_clicked();
    void on_pushButton_25_clicked();
    void on_pushButton_21_clicked();
    void on_tableWidget_5_cellClicked(int row, int column);
    void on_lineEdit_31_textChanged(const QString& text);
    void on_comboBox_6_currentIndexChanged(int index);

private:
    Ui::Gemploye *ui;
    Materiel Mtmp;
    QString selectedReference; // pour materiel
    QString selectedCin;       // pour employe
    int selectedCreateurId;    // pour createur
    QChartView *createurChartView;

    void clearCreateurForm();
    void refreshCreateurTable();
    void updateCreateurStats();
    void exportCreateurToPDF();
};

#endif // GEMPLOYE_H
