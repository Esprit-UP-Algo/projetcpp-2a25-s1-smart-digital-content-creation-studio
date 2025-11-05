#include "employe.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlError>
#include <QObject>

// Constructeurs
employee::employee() {
    cin = 0; nom = ""; prenom = ""; email = ""; mdp = ""; n_tel = 0;
}

employee::employee(int cin, QString nom, QString prenom, QString email, int n_tel, QString mdp) {
    this->cin = cin;
    this->nom = nom;
    this->prenom = prenom;
    this->email = email;
    this->n_tel = n_tel;
    this->mdp = mdp;
}

// Ajouter un employé
bool employee::ajouter()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Erreur base non ouverte:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO EMPLOYE (CIN, NOM, PRENOM, EMAIL, N_TEL, MDP) "
                  "VALUES (:cin, :nom, :prenom, :email, :n_tel, :mdp)");
    query.bindValue(":cin", cin);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":email", email);
    query.bindValue(":n_tel", n_tel);
    query.bindValue(":mdp", mdp);

    if (!query.exec()) {
        qDebug() << "Erreur SQL ajout:" << query.lastError().text();
        return false;
    }

    qDebug() << "Ajout effectué avec succès";
    return true;
}

// Modifier un employé par ID
bool employee::modifier(int id_employe, int newCin, QString newNom, QString newPrenom, QString newEmail, int newTel, QString newMdp)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE EMPLOYE SET CIN=:cin, NOM=:nom, PRENOM=:prenom, EMAIL=:email, N_TEL=:n_tel, MDP=:mdp "
                  "WHERE ID_EMPLOYE=:id");
    query.bindValue(":cin", newCin);
    query.bindValue(":nom", newNom);
    query.bindValue(":prenom", newPrenom);
    query.bindValue(":email", newEmail);
    query.bindValue(":n_tel", newTel);
    query.bindValue(":mdp", newMdp);
    query.bindValue(":id", id_employe);

    if (!query.exec()) {
        qDebug() << "Erreur SQL modification:" << query.lastError().text();
        return false;
    }

    qDebug() << "Modification réussie";
    return true;
}

// Supprimer un employé par ID
bool employee::supprimer(int cin)
{
    QSqlQuery query;
    query.prepare("DELETE FROM EMPLOYE WHERE CIN = :cin");
    query.bindValue(":cin", cin);

    return query.exec();
}

// Afficher tous les employés
QSqlQueryModel* employee::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP FROM EMPLOYE");

    // Vérifie si la requête s’est bien exécutée
    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL afficher() :" << model->lastError().text();
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID_EMPLOYE"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("CIN"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Prenom"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("N_TEL"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("MDP"));

    return model;
}
