#ifndef GESTION_SPONSOR_H
#define GESTION_SPONSOR_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Gestion_Sponsor
{
    int id_sponsor;
    int code_unique;
    QString nom_sponsor;
    QString secteur_activite;
    int montant;

public:
    Gestion_Sponsor();
    Gestion_Sponsor(int, int, QString, QString, int);

    int getId_Sponsor() { return id_sponsor; }
    int getCode_Unique() { return code_unique; }
    QString getNom_Sponsor() { return nom_sponsor; }
    QString getSecteur_Activite() { return secteur_activite; }
    int getMontant() { return montant; }

    void setId_Sponsor(int id) { id_sponsor = id; }
    void setCode_Unique(int code) { code_unique = code; }
    void setNom_Sponsor(QString nom) { nom_sponsor = nom; }
    void setSecteur_Activite(QString secteur) { secteur_activite = secteur; }
    void setMontant(int mont) { montant = mont; }

    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool modifier(int);
    QSqlQueryModel* rechercher(QString);
    QSqlQueryModel* trier(QString);
};

#endif // GESTION_SPONSOR_H
