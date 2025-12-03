#include "gestion_sponsor.h"
#include <QDebug>

Gestion_Sponsor::Gestion_Sponsor()
{
    id_sponsor = 0;
    code_unique = 0;
    nom_sponsor = "";
    secteur_activite = "";
    montant = 0;
}

Gestion_Sponsor::Gestion_Sponsor(int id, int code, QString nom, QString secteur, int mont)
{
    this->id_sponsor = id;
    this->code_unique = code;
    this->nom_sponsor = nom;
    this->secteur_activite = secteur;
    this->montant = mont;
}

bool Gestion_Sponsor::ajouter()
{
    QSqlQuery query;

    query.prepare("INSERT INTO SPONSOR (ID_SPONSOR, CODE_UNIQUE, NOM_SPONSOR, SECTEUR_ACTIVITÉ, MONTANT) "
                  "VALUES (:id, :code, :nom, :secteur, :montant)");

    query.bindValue(":id", id_sponsor);
    query.bindValue(":code", code_unique);
    query.bindValue(":nom", nom_sponsor);
    query.bindValue(":secteur", secteur_activite);
    query.bindValue(":montant", montant);

    return query.exec();
}

QSqlQueryModel* Gestion_Sponsor::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();

    model->setQuery("SELECT ID_SPONSOR, CODE_UNIQUE, NOM_SPONSOR, SECTEUR_ACTIVITÉ, MONTANT FROM SPONSOR");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Code Unique"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Nom Sponsor"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Secteur Activité"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Montant"));

    return model;
}

bool Gestion_Sponsor::supprimer(int id)
{
    QSqlQuery query;

    query.prepare("DELETE FROM SPONSOR WHERE ID_SPONSOR = :id");
    query.bindValue(":id", id);

    return query.exec();
}

bool Gestion_Sponsor::modifier(int id)
{
    QSqlQuery query;

    query.prepare("UPDATE SPONSOR SET CODE_UNIQUE = :code, NOM_SPONSOR = :nom, "
                  "SECTEUR_ACTIVITÉ = :secteur, MONTANT = :montant "
                  "WHERE ID_SPONSOR = :id");

    query.bindValue(":code", code_unique);
    query.bindValue(":nom", nom_sponsor);
    query.bindValue(":secteur", secteur_activite);
    query.bindValue(":montant", montant);
    query.bindValue(":id", id);

    return query.exec();
}

QSqlQueryModel* Gestion_Sponsor::rechercher(QString critere)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    QString queryStr = "SELECT ID_SPONSOR, CODE_UNIQUE, NOM_SPONSOR, SECTEUR_ACTIVITÉ, MONTANT "
                       "FROM SPONSOR WHERE NOM_SPONSOR LIKE '%" + critere + "%' "
                                   "OR SECTEUR_ACTIVITÉ LIKE '%" + critere + "%'";

    model->setQuery(queryStr);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Code Unique"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Nom Sponsor"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Secteur Activité"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Montant"));

    return model;
}

QSqlQueryModel* Gestion_Sponsor::trier(QString critere)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    QString queryStr = "SELECT ID_SPONSOR, CODE_UNIQUE, NOM_SPONSOR, SECTEUR_ACTIVITÉ, MONTANT "
                       "FROM SPONSOR ORDER BY " + critere;

    model->setQuery(queryStr);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Code Unique"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Nom Sponsor"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Secteur Activité"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Montant"));

    return model;
}
