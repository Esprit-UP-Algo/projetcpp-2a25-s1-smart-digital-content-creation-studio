#ifndef GEMPLOYE_H
#define GEMPLOYE_H
#include "employe.h"

#include <QMainWindow>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Gemploye;
}
QT_END_NAMESPACE

class Gprojet; // déclaration anticipée de la fenêtre de gestion projet

class Gemploye : public QMainWindow
{
    Q_OBJECT

public:
    Gemploye(QWidget *parent = nullptr);
    ~Gemploye();
    
    void configurePermissions(const QString &role);
    // Méthode d'initialisation appelée après la connexion BD
    void initAfterConnect();

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

    void on_btnCaptureFace_clicked();

    void on_btnDeconnexion_clicked();

    void on_btnDeconnexion_2_clicked();

private:
    Ui::Gemploye *ui;
    employee e_global;
    int selectedId = 0;  // instance globale pour CRUD
    bool validerChamps(QString &erreur);

    void afficherStatistiques();
    void captureFaceImage();
    
    QImage m_capturedFaceImage; // Stocke la photo capturée

    // Fenêtre de gestion des projets (ouverte via le bouton "Projet")
    Gprojet *m_projetWindow = nullptr;

};
#endif // GEMPLOYE_H
