#include "contrat.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QCryptographicHash>
#include <QDateTime>


Contrat::Contrat() {}

Contrat::Contrat(int id_contrat, const QString& type, int code_contrat, double prix,
                 const QDate& date_signature, const QDate& echeance, int id_sponsor, int id_createur)
    : id_contrat(id_contrat), type(type), code_contrat(code_contrat), prix(prix),
    date_signature(date_signature), echeance(echeance), id_sponsor(id_sponsor), id_createur(id_createur) {}

bool Contrat::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO SMART_CONTENT.CONTRAT "
                  "(ID_CONTRAT, TYPE, CODE_CONTRAT, PRIX, DATE_SIGNATURE, ECHEANCE, ID_SPONSOR, ID_CREATEUR) "
                  "VALUES (:id, :type, :code, :prix, TO_DATE(:date_signature,'YYYY-MM-DD'), "
                  "TO_DATE(:echeance,'YYYY-MM-DD'), :id_sponsor, :id_createur)");

    query.bindValue(":id", id_contrat);
    query.bindValue(":type", type);
    query.bindValue(":code", code_contrat);
    query.bindValue(":prix", prix);
    query.bindValue(":date_signature", date_signature.toString("yyyy-MM-dd"));
    query.bindValue(":echeance", echeance.toString("yyyy-MM-dd"));
    query.bindValue(":id_sponsor", id_sponsor);
    query.bindValue(":id_createur", id_createur);

    if(!query.exec()) {
        qDebug() << "Erreur ajout contrat:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Contrat::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE SMART_CONTENT.CONTRAT SET "
                  "TYPE=:type, CODE_CONTRAT=:code, PRIX=:prix, "
                  "DATE_SIGNATURE=TO_DATE(:date_signature,'YYYY-MM-DD'), "
                  "ECHEANCE=TO_DATE(:echeance,'YYYY-MM-DD'), "
                  "ID_SPONSOR=:id_sponsor, ID_CREATEUR=:id_createur "
                  "WHERE ID_CONTRAT=:id");

    query.bindValue(":type", type);
    query.bindValue(":code", code_contrat);
    query.bindValue(":prix", prix);
    query.bindValue(":date_signature", date_signature.toString("yyyy-MM-dd"));
    query.bindValue(":echeance", echeance.toString("yyyy-MM-dd"));
    query.bindValue(":id_sponsor", id_sponsor);
    query.bindValue(":id_createur", id_createur);
    query.bindValue(":id", id_contrat);

    if(!query.exec()) {
        qDebug() << "Erreur modification contrat:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Contrat::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM SMART_CONTENT.CONTRAT WHERE ID_CONTRAT=:id");
    query.bindValue(":id", id);

    if(!query.exec()) {
        qDebug() << "Erreur suppression contrat:" << query.lastError().text();
        return false;
    }
    return true;
}

void Contrat::rechercherParType(QTableWidget* table, const QString& type)
{
    if(!table) return;

    table->clear();
    table->setRowCount(0);
    table->setColumnCount(7);
    QStringList headers = {"ID_CONTRAT", "ID_SPONSOR", "ID_CREATEUR", "TYPE", "PRIX", "DATE_SIGNATURE", "ECHEANCE"};
    table->setHorizontalHeaderLabels(headers);

    QSqlQuery query;
    query.prepare("SELECT ID_CONTRAT, ID_SPONSOR, ID_CREATEUR, TYPE, PRIX, "
                  "TO_CHAR(DATE_SIGNATURE,'YYYY-MM-DD'), TO_CHAR(ECHEANCE,'YYYY-MM-DD') "
                  "FROM SMART_CONTENT.CONTRAT WHERE LOWER(TYPE) LIKE LOWER(:type) ORDER BY ID_CONTRAT");
    query.bindValue(":type", "%" + type + "%");

    if(!query.exec()) {
        qDebug() << "Erreur recherche contrat:" << query.lastError().text();
        return;
    }

    int row = 0;
    while(query.next()) {
        table->insertRow(row);
        for(int col=0; col<7; col++)
            table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        row++;
    }
    table->resizeColumnsToContents();
}

void Contrat::trierParPrix(QTableWidget *table, bool croissant)
{
    if (!table) return;

    table->clear();
    table->setRowCount(0);
    table->setColumnCount(7);

    QStringList headers = {"ID_CONTRAT", "ID_SPONSOR", "ID_CREATEUR",
                           "TYPE", "PRIX", "DATE_SIGNATURE", "ECHEANCE"};
    table->setHorizontalHeaderLabels(headers);

    QString ordre = croissant ? "ASC" : "DESC";

    QSqlQuery query(
        "SELECT ID_CONTRAT, ID_SPONSOR, ID_CREATEUR, TYPE, PRIX, "
        "TO_CHAR(DATE_SIGNATURE,'YYYY-MM-DD'), "
        "TO_CHAR(ECHEANCE,'YYYY-MM-DD') "
        "FROM SMART_CONTENT.CONTRAT ORDER BY PRIX " + ordre
        );

    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < 7; col++)
            table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        row++;
    }

    table->resizeColumnsToContents();
}

void Contrat::afficher(QTableWidget* table)
{
    if(!table) return;

    table->clear();
    table->setRowCount(0);
    table->setColumnCount(7);
    QStringList headers = {"ID_CONTRAT", "ID_SPONSOR", "ID_CREATEUR", "TYPE", "PRIX", "DATE_SIGNATURE", "ECHEANCE"};
    table->setHorizontalHeaderLabels(headers);

    QSqlQuery query("SELECT ID_CONTRAT, ID_SPONSOR, ID_CREATEUR, TYPE, PRIX, "
                    "TO_CHAR(DATE_SIGNATURE,'YYYY-MM-DD'), TO_CHAR(ECHEANCE,'YYYY-MM-DD') "
                    "FROM SMART_CONTENT.CONTRAT ORDER BY ID_CONTRAT");


    int row = 0;
    while(query.next()) {
        table->insertRow(row);
        for(int col=0; col<7; col++)
            table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        row++;
    }
    table->resizeColumnsToContents();
}
