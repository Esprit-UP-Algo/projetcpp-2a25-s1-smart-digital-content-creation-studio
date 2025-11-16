#include "employe.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlError>
#include <QObject>

// Constructeurs
employee::employee() {
}

employee::employee(int cin, QString nom, QString prenom,
                   QString email, int n_tel, QString mdp,
                   QString poste, double salaire) {
    this->cin = cin;
    this->nom = nom;
    this->prenom = prenom;
    this->email = email;
    this->n_tel = n_tel;
    this->mdp = mdp;
    this->poste = poste;
    this->salaire = salaire;
}

// Getters
int employee::getCin() const { return cin; }
QString employee::getNom() const { return nom; }
QString employee::getPrenom() const { return prenom; }
QString employee::getEmail() const { return email; }
int employee::getTel() const { return n_tel; }
QString employee::getMdp() const { return mdp; }
QString employee::getPoste() const { return poste; }
double employee::getSalaire() const { return salaire; }

// Setters
void employee::setCin(int c) { cin = c; }
void employee::setNom(QString n) { nom = n; }
void employee::setPrenom(QString p) { prenom = p; }
void employee::setEmail(QString e) { email = e; }
void employee::setTel(int t) { n_tel = t; }
void employee::setMdp(QString m) { mdp = m; }
void employee::setPoste(QString p) { poste = p; }
void employee::setSalaire(double s) { salaire = s; }

// Ajouter un employé
bool employee::ajouter(int cin, QString nom, QString prenom,
                       QString email, int n_tel, QString mdp,
                       QString poste, double salaire)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Erreur base non ouverte:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO EMPLOYE (ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, SALAIRE) "
                  "VALUES (EMPLOYE_SEQ.NEXTVAL, :cin, :nom, :prenom, :email, :n_tel, :mdp, :poste, :salaire)");

    query.bindValue(":cin", cin);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":email", email);
    query.bindValue(":n_tel", n_tel);
    query.bindValue(":mdp", mdp);
    query.bindValue(":poste", poste);
    query.bindValue(":salaire", salaire);

    if (!query.exec()) {
        qDebug() << "Erreur SQL ajout:" << query.lastError().text();
        return false;
    }

    qDebug() << "Ajout effectué avec succès";
    return true;
}


// Modifier un employé par ID
bool employee::modifier(int id_employe, int newCin, QString newNom, QString newPrenom,
                        QString newEmail, int newTel, QString newMdp,
                        QString newPoste, double newSalaire)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE EMPLOYE SET CIN=:cin, NOM=:nom, PRENOM=:prenom, EMAIL=:email, "
                  "N_TEL=:n_tel, MDP=:mdp, POSTE=:poste, SALAIRE=:salaire "
                  "WHERE ID_EMPLOYE=:id");
    query.bindValue(":cin", newCin);
    query.bindValue(":nom", newNom);
    query.bindValue(":prenom", newPrenom);
    query.bindValue(":email", newEmail);
    query.bindValue(":n_tel", newTel);
    query.bindValue(":mdp", newMdp);
    query.bindValue(":poste", newPoste);
    query.bindValue(":salaire", newSalaire);
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
    model->setQuery("SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, SALAIRE FROM EMPLOYE");

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
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("POSTE"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("SALAIRE"));

    return model;
}

QSqlQueryModel* employee::afficherTrieParSalaire()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, SALAIRE FROM EMPLOYE ORDER BY SALAIRE DESC");

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL afficherTrieParSalaire() :" << model->lastError().text();
    }

    return model;
}

QSqlQueryModel* employee::afficherTrieParNom()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, SALAIRE FROM EMPLOYE ORDER BY NOM ASC");

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL afficherTrieParNom() :" << model->lastError().text();
    }

    return model;
}

QSqlQueryModel* employee::rechercherParCin(const QString &cinPartiel)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    QString queryStr =
        "SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, SALAIRE "
        "FROM EMPLOYE "
        "WHERE TO_CHAR(CIN) LIKE '" + cinPartiel + "%'";

    model->setQuery(queryStr);

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL rechercherParCin() :" << model->lastError().text();
    }

    return model;
}

bool employee::existe(int cin)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM EMPLOYE WHERE CIN = :cin");
    query.bindValue(":cin", cin);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
