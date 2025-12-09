#ifndef GEMPLOYE_H
#define GEMPLOYE_H
#include "employe.h"
#include "arduino.h"

#include <QMainWindow>
<<<<<<< Updated upstream
=======
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QVideoWidget>
#include <QTimer>

<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes

QT_BEGIN_NAMESPACE
namespace Ui {
class Gemploye;
}
QT_END_NAMESPACE

<<<<<<< Updated upstream
=======
class Gprojet; // déclaration anticipée de la fenêtre de gestion projet
class Gcontrat; // déclaration anticipée de la fenêtre de gestion contrat
class Gsponsor; // déclaration anticipée de la fenêtre de gestion sponsor

>>>>>>> Stashed changes
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

private:
    Ui::Gemploye *ui;
    employee e_global;
    int selectedId = 0;  // instance globale pour CRUD

    // Fenêtre de gestion des contrats (intégrée via le bouton "Contrat")
    Gcontrat *m_contratWindow = nullptr;

    // Fenêtre de gestion des contrats (intégrée via le bouton "Contrat")
    Gsponsor *m_sponsorWindow = nullptr;

<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
    // Fenêtre de gestion des contrats (intégrée via le bouton "Contrat")
    Gcontrat *m_contratWindow = nullptr;

    // Fenêtre de gestion des contrats (intégrée via le bouton "Contrat")
    Gsponsor *m_sponsorWindow = nullptr;

<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
    // Arduino
    Arduino *arduino = nullptr;
    QTimer *arduinoTimer = nullptr;
};
#endif // GEMPLOYE_H
