#include "projet.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

// Constructeur par défaut
Projet::Projet()
{
    code = "";
    titre = "";
    budget = 0.0;
    deadline = "";
}

// Constructeur avec paramètres
Projet::Projet(QString code, QString titre, double budget, QString deadline)
{
    this->code = code;
    this->titre = titre;
    this->budget = budget;
    this->deadline = deadline;
}

QSqlQueryModel* Projet::trierSelonColonne(const QString& colonne, const QString& ordre)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    const QString statement = QStringLiteral(
        "SELECT CODE, TITRE, BUDGET, DEADLINE FROM PROJET ORDER BY %1 %2")
        .arg(colonne, ordre);

    model->setQuery(statement);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Code"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Budget"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Deadline"));

    return model;
}

// Ajouter un projet
bool Projet::ajouter()
{
    QSqlQuery query;
    
    // Get the next ID_PROJET by finding the maximum and adding 1
    query.prepare("SELECT NVL(MAX(ID_PROJET), 0) + 1 FROM PROJET");
    if (!query.exec())
    {
        qDebug() << "Erreur lors de la récupération du prochain ID:" << query.lastError().text();
        return false;
    }
    
    int nextId = 1;
    if (query.next())
    {
        nextId = query.value(0).toInt();
    }
    
    // Insert with ID_PROJET included
    query.prepare(
        "INSERT INTO PROJET (ID_PROJET, CODE, TITRE, BUDGET, DEADLINE) "
        "VALUES (:id_projet, :code, :titre, :budget, TO_DATE(:deadline, 'YYYY-MM-DD'))");

    query.bindValue(":id_projet", nextId);
    query.bindValue(":code", code);
    query.bindValue(":titre", titre);
    query.bindValue(":budget", budget);
    query.bindValue(":deadline", deadline);

    if (query.exec())
    {
        qDebug() << "Projet ajouté avec succès";
        return true;
    }

    qDebug() << "Erreur lors de l'ajout:" << query.lastError().text();
    return false;
}

// Modifier un projet
bool Projet::modifier()
{
    QSqlQuery query;
    query.prepare(
        "UPDATE PROJET SET TITRE = :titre, BUDGET = :budget, "
        "DEADLINE = TO_DATE(:deadline, 'YYYY-MM-DD') WHERE CODE = :code");

    query.bindValue(":code", code);
    query.bindValue(":titre", titre);
    query.bindValue(":budget", budget);
    query.bindValue(":deadline", deadline);

    if (!query.exec())
    {
        qDebug() << "Erreur lors de la modification:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "Aucun projet mis à jour pour le code" << code;
        return false;
    }

    qDebug() << "Projet modifié avec succès";
    return true;
}

// Supprimer un projet
bool Projet::supprimer(const QString& code)
{
    QSqlQuery query;
    query.prepare("DELETE FROM PROJET WHERE CODE = :code");
    query.bindValue(":code", code);

    if (!query.exec())
    {
        qDebug() << "Erreur lors de la suppression:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "Aucun projet supprimé pour le code" << code;
        return false;
    }

    qDebug() << "Projet supprimé avec succès";
    return true;
}

// Afficher tous les projets
QSqlQueryModel* Projet::afficher()
{
    return trierSelonColonne(QStringLiteral("ID_PROJET"), QStringLiteral("DESC"));
}

// Rechercher un projet par code
bool Projet::rechercherParCode(const QString& code)
{
    QSqlQuery query;
    query.prepare("SELECT CODE, TITRE, BUDGET, DEADLINE FROM PROJET WHERE CODE = :code");
    query.bindValue(":code", code);

    if (query.exec() && query.next())
    {
        this->code = query.value(0).toString();
        this->titre = query.value(1).toString();
        this->budget = query.value(2).toDouble();
        this->deadline = query.value(3).toString();
        return true;
    }

    return false;
}

QSqlQueryModel* Projet::rechercher(const QString& motCle)
{
    QSqlQuery query;
    QSqlQueryModel* model = new QSqlQueryModel();

    query.prepare(
        "SELECT CODE, TITRE, BUDGET, DEADLINE "
        "FROM PROJET "
        "WHERE LOWER(CODE) LIKE LOWER(:motcle) "
        "   OR LOWER(TITRE) LIKE LOWER(:motcle)");

    query.bindValue(":motcle", QStringLiteral("%%1%").arg(motCle));

    if (!query.exec())
    {
        qDebug() << "Erreur lors de la recherche:" << query.lastError().text();
        delete model;
        return nullptr;
    }

    model->setQuery(query);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Code"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Budget"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Deadline"));

    return model;
}

// Trier par code
QSqlQueryModel* Projet::trierParCode()
{
    return trierSelonColonne(QStringLiteral("CODE"));
}

// Trier par titre
QSqlQueryModel* Projet::trierParTitre()
{
    return trierSelonColonne(QStringLiteral("TITRE"));
}

// Trier par budget
QSqlQueryModel* Projet::trierParBudget()
{
    return trierSelonColonne(QStringLiteral("BUDGET"), QStringLiteral("DESC"));
}

// Trier par deadline
QSqlQueryModel* Projet::trierParDeadline()
{
    return trierSelonColonne(QStringLiteral("DEADLINE"));
}

// Afficher les projets avec deadline dans les 7 prochains jours
QSqlQueryModel* Projet::afficherProjetsUrgents()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;
    
    query.prepare(
        "SELECT CODE, TITRE, BUDGET, TO_CHAR(DEADLINE, 'YYYY-MM-DD') AS DEADLINE "
        "FROM PROJET "
        "WHERE DEADLINE <= SYSDATE + 7 "
        "ORDER BY DEADLINE ASC");
    
    if (!query.exec())
    {
        qDebug() << "Erreur lors de la récupération des projets urgents:" << query.lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setQuery(query);
    
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Code"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Budget"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Deadline"));
    
    return model;
}

