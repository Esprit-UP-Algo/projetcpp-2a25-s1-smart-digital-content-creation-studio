#include "materiel.h"
#include <QTableWidgetItem>

// Constructeurs
Materiel::Materiel()
    : type_appareil(""), reference(""), disponibilite(""), marque("")
{}

Materiel::Materiel(const QString& type, const QString& ref, const QString& m, const QString& dispo)
    : type_appareil(type), reference(ref), disponibilite(dispo), marque(m)
{}

// Ajouter un matériel
bool Materiel::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO MATERIEL (TYPE_APPAREIL, REFERENCE, MARQUE, DISPONIBILITE) "
                  "VALUES (:type, :ref, :marque, :dispo)");
    query.bindValue(":type", type_appareil);
    query.bindValue(":ref", reference);
    query.bindValue(":marque", marque);
    query.bindValue(":dispo", disponibilite);

    if (query.exec()) {
        qDebug() << "SUCCES: Materiel ajoute!";
        return true;
    } else {
        qDebug() << "ERREUR SQL (ajouter):" << query.lastError().text();
        return false;
    }
}

// Afficher tous les matériels
bool Materiel::afficher(QTableWidget *table)
{
    if (!table) return false;

    QSqlQuery query("SELECT TYPE_APPAREIL, REFERENCE, MARQUE, DISPONIBILITE FROM MATERIEL");

    table->clear();
    table->setRowCount(0);
    int row = 0;

    while (query.next()) {
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // TYPE_APPAREIL
        table->setItem(row, 1, new QTableWidgetItem(query.value(1).toString())); // REFERENCE
        table->setItem(row, 2, new QTableWidgetItem(query.value(2).toString())); // MARQUE
        table->setItem(row, 3, new QTableWidgetItem(query.value(3).toString())); // DISPONIBILITE
        ++row;
    }
    return true;
}

// Modifier un matériel (mettre a jour TOUS les attributs, y compris REFERENCE)
// ref_old: ancienne valeur de la colonne REFERENCE pour trouver la ligne
bool Materiel::modifier(const QString& ref_old)
{
    QSqlQuery query;
    query.prepare("UPDATE MATERIEL "
                  "SET TYPE_APPAREIL = :type, "
                  "    REFERENCE     = :newref, "
                  "    MARQUE        = :marque, "
                  "    DISPONIBILITE = :dispo "
                  "WHERE REFERENCE   = :oldref");

    query.bindValue(":type",   type_appareil);
    query.bindValue(":newref", reference);       // nouvelle référence (depuis this->reference)
    query.bindValue(":marque", marque);
    query.bindValue(":dispo",  disponibilite);
    query.bindValue(":oldref", ref_old);         // ancienne référence (clé)

    if (!query.exec()) {
        qDebug() << "ERREUR SQL (modifier):" << query.lastError().text();
        return false;
    }
    // Optionnel: vérifier qu'au moins une ligne a été touchée
    // return (query.numRowsAffected() > 0);
    return true;
}

// Supprimer un matériel (basé sur REFERENCE)
bool Materiel::supprimer(const QString& ref)
{
    QSqlQuery query;
    query.prepare("DELETE FROM MATERIEL WHERE REFERENCE = :ref");
    query.bindValue(":ref", ref);

    if (query.exec()) {
        qDebug() << "SUCCES: Materiel supprime!";
        return true;
    } else {
        qDebug() << "ERREUR SQL (supprimer):" << query.lastError().text();
        return false;
    }
}
