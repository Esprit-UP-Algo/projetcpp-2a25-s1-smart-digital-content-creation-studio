// Fenêtre de gestion des projets (séparée de la gestion des employés)
#ifndef GPROJET_H
#define GPROJET_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>
#include <QSystemTrayIcon>
#include <memory>

#include "projet.h"
#include "arduino.h"
#include "qrcodegenerator.h"
#include "qrcodescanner.h"
#ifdef HAVE_CHARTS
#include <QtCharts/QChartView>
#endif

class QSqlQueryModel;

QT_BEGIN_NAMESPACE
namespace Ui {
class Gprojet;
}
QT_END_NAMESPACE

class Gprojet : public QMainWindow
{
    Q_OBJECT

public:
    Gprojet(QWidget *parent = nullptr);
    ~Gprojet();
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
    void on_pushButton_23_clicked(); // Exporter PDF
    void on_tableWidget_2_cellClicked(int row, int column);
    void on_lineEdit_16_textChanged(const QString& text);
    void on_notificationButton_clicked();
    void on_logo_clicked();
    void on_connectArduino_clicked();
    void on_sendToArduino_clicked();
    void on_generateQRCode_clicked();
    void on_scanQRCode_clicked();
    void readArduinoData();
    void on_btnDeconnexionProjet_clicked();

private:
    void initialiserProjetUi();
    void chargerTableProjets(QSqlQueryModel* model = nullptr);
    void viderFormulaireProjet();
    bool verifierChampsProjet(QString& message) const;
    void afficherMessageProjet(const QString& message, QMessageBox::Icon icon = QMessageBox::Information);
    void remplirFormulaireDepuisLigne(int row);
    void rafraichirStatistiquesProjet();
    void verifierDeadlinesProjets();
    void afficherNotifications();
    void afficherNotificationsWindows();
    void mettreAJourBadgeNotification(int count);
    void generateProjectQRCode(int row);
    void mettreAJourQRCode();
    void exporterProjetsPDF();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void testArduinoConnection();

    Ui::Gprojet *ui;
    Projet projetCourant;
    QRCodeGenerator *qrGenerator;
    QSystemTrayIcon *systemTrayIcon;
#ifdef HAVE_CHARTS
    QChartView* chartViewProjet = nullptr;
#endif
    int nombreNotifications = 0;
    Arduino* arduino = nullptr;
};
#endif // GPROJET_H
