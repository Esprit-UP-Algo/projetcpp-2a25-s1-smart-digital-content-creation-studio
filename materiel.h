#ifndef MATERIEL_H
#define MATERIEL_H

#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QTableWidget>

class Materiel
{
private:
    QString type_appareil;
    QString reference;      // nouvelle valeur de REFERENCE (si elle change)
    QString disponibilite;
    QString marque;

public:
    Materiel();
    Materiel(const QString& type, const QString& ref, const QString& marque, const QString& dispo);

    bool ajouter();
    bool afficher(QTableWidget *table);

    // ref_old = ancienne REFERENCE pour le WHERE, les nouvelles valeurs viennent des membres
    bool modifier(const QString& ref_old);
    bool supprimer(const QString& ref);
};

#endif // MATERIEL_H
