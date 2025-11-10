#ifndef GEMPLOYE_H
#define GEMPLOYE_H

#include <QMainWindow>
#include <QMessageBox>
#include <memory>

#include "projet.h"
#include <QtCharts/QChartView>

class QSqlQueryModel;

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
    void initAfterConnect();

private slots:
    void on_Employ_clicked();

    void on_Employ_4_clicked();

    void on_Employ_2_clicked();

    void on_Employ_6_clicked();

    void on_Employ_3_clicked();

    void on_Employ_5_clicked();

    void on_pushButton_32_clicked(); // Valider (Ajouter/Modifier)
    void on_pushButton_33_clicked(); // Annuler
    void on_pushButton_13_clicked(); // Supprimer
    void on_pushButton_12_clicked(); // Recherche
    void on_pushButton_6_clicked();  // Tri
    void on_tableWidget_2_cellClicked(int row, int column);
    void on_lineEdit_16_textChanged(const QString& text);

private:
    void initialiserProjetUi();
    void chargerTableProjets(QSqlQueryModel* model = nullptr);
    void viderFormulaireProjet();
    bool verifierChampsProjet(QString& message) const;
    void afficherMessageProjet(const QString& message, QMessageBox::Icon icon = QMessageBox::Information);
    void remplirFormulaireDepuisLigne(int row);
    void rafraichirStatistiquesProjet();

    Ui::Gemploye *ui;
    Projet projetCourant;
    QChartView* chartViewProjet = nullptr;
};
#endif // GEMPLOYE_H
