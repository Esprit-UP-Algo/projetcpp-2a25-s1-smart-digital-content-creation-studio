#ifndef MATRIEL_H
#define MATRIEL_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>

class Materiel
{
public:
    // Constructeurs
    Materiel();

    // CRUD
    bool ajouter(QString code, QString type, QString marque, QString ref, QString dispo);
    bool modifier(QString code, QString type, QString marque, QString ref, QString dispo);
    bool supprimer(QString code);
    QSqlQueryModel* afficher();
    bool existe(QString code);

    // Méthodes d'association
    bool assignerAEmploye(QString code_materiel, int id_employe);
    bool libererMateriel(QString code_materiel);
    QSqlQueryModel* afficherMaterielsEmploye(int id_employe);
    QSqlQueryModel* afficherMaterielsDisponibles();
};

#endif // MATRIEL_H
