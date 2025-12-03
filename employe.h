#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QString>
#include <QSqlQueryModel>
#include <QByteArray>
#include <QImage>

class employee
{
private:
    int id_employe;  // auto-increment dans la base
    int cin;
    int n_tel;
    QString nom;
    QString prenom;
    QString email;
    QString mdp;
    QString poste;
    QString role;
    double salaire;

public:
    employee();
    employee(int cin, QString nom, QString prenom,
             QString email, int n_tel, QString mdp,
             QString poste, QString role, double salaire);

    // Getters
    int getCin() const;
    QString getNom() const;
    QString getPrenom() const;
    QString getEmail() const;
    int getTel() const;
    QString getMdp() const;
    QString getPoste() const;
    QString getRole() const;
    double getSalaire() const;

    // Setters
    void setCin(int);
    void setNom(QString);
    void setPrenom(QString);
    void setEmail(QString);
    void setTel(int);
    void setMdp(QString);
    void setPoste(QString);
    void setRole(QString);
    void setSalaire(double);

    // CRUD
    bool ajouter(int cin, QString nom, QString prenom,
                 QString email, int n_tel, QString mdp,
                 QString poste, QString role, double salaire);
    bool modifier(int id_employe, int newCin, QString newNom, QString newPrenom,
                  QString newEmail, int newTel, QString newMdp,
                  QString newPoste, QString newRole, double newSalaire);
    QSqlQueryModel* afficher();
    bool supprimer(int cin);
    bool existe(int cin);

    // Affichages avancés
    QSqlQueryModel* afficherTrieParSalaire();
    QSqlQueryModel* afficherTrieParNom();
    QSqlQueryModel* rechercherParCin(const QString &cinPartiel);

    // Face ID
    static bool saveFaceImage(const QString &email, const QImage &faceImage);
    static QImage getFaceImage(const QString &email);
    static QString authenticateByFace(const QImage &capturedImage);
    static int compareImages(const QImage &img1, const QImage &img2);
};

#endif // EMPLOYEE_H
