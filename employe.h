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

private:
    int id_employe;  // auto-increment dans la base
    int cin;
    int n_tel;
    QString nom;
    QString prenom;
    QString email;
    QString mdp;
};

#endif // EMPLOYEE_H
