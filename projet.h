#ifndef PROJET_H
#define PROJET_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QObject>

class Projet
{
private:
    QString code;
    QString titre;
    double budget;
    QString deadline;

    QSqlQueryModel* trierSelonColonne(const QString& colonne, const QString& ordre = QStringLiteral("ASC"));

public:
    // Constructeurs
    Projet();
    Projet(QString code, QString titre, double budget, QString deadline);

    // Getters
    QString getCode() const { return code; }
    QString getTitre() const { return titre; }
    double getBudget() const { return budget; }
    QString getDeadline() const { return deadline; }

    // Setters
    void setCode(QString c) { code = c; }
    void setTitre(QString t) { titre = t; }
    void setBudget(double b) { budget = b; }
    void setDeadline(QString d) { deadline = d; }

    // Méthodes CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(const QString& code);
    QSqlQueryModel* afficher();

    // Méthodes supplémentaires
    bool rechercherParCode(const QString& code);
    QSqlQueryModel* rechercher(const QString& motCle);
    QSqlQueryModel* trierParCode();
    QSqlQueryModel* trierParTitre();
    QSqlQueryModel* trierParBudget();
    QSqlQueryModel* trierParDeadline();
    QSqlQueryModel* afficherProjetsUrgents();
};

#endif // PROJET_H
