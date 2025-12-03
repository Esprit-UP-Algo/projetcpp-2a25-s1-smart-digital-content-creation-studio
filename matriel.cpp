#include "matriel.h"

Materiel::Materiel()
{
}

// Ajouter un matériel
bool Materiel::ajouter(QString code, QString type, QString marque, QString ref, QString dispo)
{
    QSqlQuery query;
    query.prepare("INSERT INTO MATERIEL (CODE_UNIQUE, TYPE_APPAREIL, MARQUE, REFERENCE, DISPONIBILITE) "
                  "VALUES (:code, :type, :marque, :ref, :dispo)");

    query.bindValue(":code", code);
    query.bindValue(":type", type);
    query.bindValue(":marque", marque);
    query.bindValue(":ref", ref);
    query.bindValue(":dispo", dispo);

    if (!query.exec()) {
        qDebug() << "Erreur SQL ajout matériel:" << query.lastError().text();
        return false;
    }

    qDebug() << "Matériel ajouté avec succès";
    return true;
}

// Modifier un matériel
bool Materiel::modifier(QString code, QString type, QString marque, QString ref, QString dispo)
{
    QSqlQuery query;
    query.prepare("UPDATE MATERIEL SET TYPE_APPAREIL=:type, MARQUE=:marque, "
                  "REFERENCE=:ref, DISPONIBILITE=:dispo WHERE CODE_UNIQUE=:code");

    query.bindValue(":type", type);
    query.bindValue(":marque", marque);
    query.bindValue(":ref", ref);
    query.bindValue(":dispo", dispo);
    query.bindValue(":code", code);

    if (!query.exec()) {
        qDebug() << "Erreur SQL modification matériel:" << query.lastError().text();
        return false;
    }

    qDebug() << "Matériel modifié avec succès";
    return true;
}

// Supprimer un matériel
bool Materiel::supprimer(QString code)
{
    QSqlQuery query;
    query.prepare("DELETE FROM MATERIEL WHERE CODE_UNIQUE = :code");
    query.bindValue(":code", code);

    if (!query.exec()) {
        qDebug() << "Erreur SQL suppression:" << query.lastError().text();
        return false;
    }

    return true;
}

// Afficher tous les matériels
QSqlQueryModel* Materiel::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT CODE_UNIQUE, TYPE_APPAREIL, MARQUE, REFERENCE, DISPONIBILITE "
                    "FROM MATERIEL");

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL afficher() :" << model->lastError().text();
    }

    model->setHeaderData(0, Qt::Horizontal, "Code");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Marque");
    model->setHeaderData(3, Qt::Horizontal, "Référence");
    model->setHeaderData(4, Qt::Horizontal, "Disponibilité");

    return model;
}

// Vérifier si un matériel existe
bool Materiel::existe(QString code)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM MATERIEL WHERE CODE_UNIQUE = :code");
    query.bindValue(":code", code);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

// Assigner un matériel à un employé
bool Materiel::assignerAEmploye(QString code_materiel, int id_employe)
{
    QSqlQuery query;
    query.prepare("UPDATE MATERIEL SET ID_EMPLOYE=:id_emp, DISPONIBILITE='non disponible' "
                  "WHERE CODE_UNIQUE=:code");
    query.bindValue(":id_emp", id_employe);
    query.bindValue(":code", code_materiel);

    if (!query.exec()) {
        qDebug() << "Erreur assignation matériel:" << query.lastError().text();
        return false;
    }

    qDebug() << "Matériel assigné à l'employé";
    return true;
}

// Libérer un matériel
bool Materiel::libererMateriel(QString code_materiel)
{
    QSqlQuery query;
    query.prepare("UPDATE MATERIEL SET ID_EMPLOYE=NULL, DISPONIBILITE='disponible' "
                  "WHERE CODE_UNIQUE=:code");
    query.bindValue(":code", code_materiel);

    if (!query.exec()) {
        qDebug() << "Erreur libération matériel:" << query.lastError().text();
        return false;
    }

    qDebug() << "Matériel libéré";
    return true;
}

// Afficher les matériels d'un employé spécifique
QSqlQueryModel* Materiel::afficherMaterielsEmploye(int id_employe)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT M.CODE_UNIQUE, M.TYPE_APPAREIL, M.MARQUE, M.REFERENCE "
                  "FROM MATERIEL M "
                  "WHERE M.ID_EMPLOYE = :id");
    query.bindValue(":id", id_employe);

    if (!query.exec()) {
        qDebug() << "Erreur affichage matériels employé:" << query.lastError().text();
    }

    model->setQuery(std::move(query));
    return model;
}

// Afficher uniquement les matériels disponibles
QSqlQueryModel* Materiel::afficherMaterielsDisponibles()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT CODE_UNIQUE, TYPE_APPAREIL, MARQUE, REFERENCE "
                    "FROM MATERIEL WHERE DISPONIBILITE = 'disponible'");

    if (model->lastError().isValid()) {
        qDebug() << "Erreur affichage matériels disponibles:" << model->lastError().text();
    }

    return model;
}
