#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QString>
#include <QSqlQueryModel>

class employee
{
public:
    employee();
    employee(int cin, QString nom, QString prenom,
             QString email, int n_tel, QString mdp);

    // Getters
    int getCin();
    QString getNom();
    QString getPrenom();
    QString getEmail();
    int getTel();
    QString getMdp();

    // Setters
    void setCin(int);
    void setNom(QString);
    void setPrenom(QString);
    void setEmail(QString);
    void setTel(int);
    void setMdp(QString);

    // CRUD
    bool ajouter();
    bool modifier(int id_employe, int newCin, QString newNom, QString newPrenom,
                  QString newEmail, int newTel, QString newMdp);  // modification par ID
    QSqlQueryModel* afficher();
    bool supprimer(int cin);

<<<<<<< Updated upstream
private:
    int id_employe;  // auto-increment dans la base
    int cin;
    int n_tel;
    QString nom;
    QString prenom;
    QString email;
    QString mdp;
=======
    // Accès simplifié pour Arduino/Qt
    static QString getRoleByCin(int cin);

    // Accès simplifié pour Arduino/Qt
    static QString getRoleByCin(int cin);

    // Accès simplifié pour Arduino/Qt
    static QString getRoleByCin(int cin);

    // Accès simplifié pour Arduino/Qt
    static QString getRoleByCin(int cin);

    // Accès simplifié pour Arduino/Qt
    static QString getRoleByCin(int cin);

    // Affichages avancés
    QSqlQueryModel* afficherTrieParSalaire();
    QSqlQueryModel* afficherTrieParNom();
    QSqlQueryModel* rechercherParCin(const QString &cinPartiel);

    // Face ID
    static bool saveFaceImage(const QString &email, const QImage &faceImage);
    static QImage getFaceImage(const QString &email);
    static QString authenticateByFace(const QImage &capturedImage);
    static int compareImages(const QImage &img1, const QImage &img2);
>>>>>>> Stashed changes
};

#endif // EMPLOYEE_H
