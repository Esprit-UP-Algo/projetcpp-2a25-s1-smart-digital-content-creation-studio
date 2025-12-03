#ifndef GEMPLOYE_H
#define GEMPLOYE_H

#include <QMainWindow>
#include "gestion_sponsor.h"
#include <QTextToSpeech>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui {
class Gemploye;
}
QT_END_NAMESPACE

class Gemploye : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gemploye(QWidget *parent = nullptr);
    ~Gemploye();

    Gemploye(const Gemploye&) = delete;
    Gemploye& operator=(const Gemploye&) = delete;

private slots:
    void on_Employ_clicked();
    void on_Employ_4_clicked();
    void on_Employ_2_clicked();
    void on_Employ_6_clicked();
    void on_Employ_3_clicked();
    void on_Employ_5_clicked();

    void on_pushButton_15_clicked();
    void on_pushButton_18_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_17_clicked();
    void on_lineEdit_25_textChanged(const QString &arg1);
    void on_comboBox_5_currentTextChanged(const QString &arg1);

    void on_lineEdit_21_textChanged(const QString &text);
    void on_lineEdit_22_textChanged(const QString &text);
    void on_lineEdit_24_textChanged(const QString &text);
    void on_lineEdit_26_textChanged(const QString &text);

    void on_tabWidget_4_currentChanged(int index);

    void on_btn_microphone_clicked();

    void on_tableWidget_4_cellDoubleClicked(int row, int column);

private:
    void actualiserTableauSponsors();
    void setupSponsorsValidators();
    void peuplerSecteursActivite();
    void peuplerStatuts();
    bool codeExistsInDatabase(int code);
    bool isValidName(const QString &name);
    bool isValidSecteur(const QString &secteur);
    bool isValidMontantFormat(const QString &montant);
    void highlightEmptyFields(const QString &code, const QString &nom);
    void resetFieldStyles();
    void clearSponsorFields();
    void afficherStatistiques();
    void afficherHistogrammeStatistiques();
    void chargerSponsorPourModification(int row);
    void annulerModification();

    // Fonction helper pour le PDF
    void drawSectionHeader(QPainter &painter, const QString &title, int x, int y, int width);

    void lancer_analyse_strategique();
    QString analyser_budget(const QString &requete);
    QString generer_conseil_strategique(int budget);
    QVector<QPair<QString, double>> analyser_secteurs();
    void parler_reponse(const QString &reponse);

    void chargerStatuts();
    void sauvegarderStatuts();
    QString getStatutSponsor(int codeSponsor);
    void setStatutSponsor(int codeSponsor, const QString &statut);

    // Fonctions utilitaires pour le calcul d'échelle
    double calculerEchelle(double valeur);
    double calculerIntervalle(double maxValue);

    Ui::Gemploye *ui;
    Gestion_Sponsor sponsor;
    QTextToSpeech *syntheseVocale;
    bool modeModification;
    int idSponsorEnCours;
    QMap<int, QString> statutsSponsors;
};

#endif // GEMPLOYE_H
