#include "createur.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QRegularExpression>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVariant>
#include <QDebug>
#include <QObject>

namespace {
constexpr int CREATEUR_COLUMNS = 6;
constexpr auto DATE_DISPLAY_FORMAT = "dd/MM/yyyy";
}

Createur::Createur()
    : m_id(-1),
      m_codeUnique(0),
      m_nom(),
      m_prenom(),
      m_dateAdhesion(QDate::currentDate()),
      m_categorie(),
      m_abonnes(0)
{}

Createur::Createur(int codeUnique,
                   const QString& nom,
                   const QString& prenom,
                   const QDate& dateAdhesion,
                   const QString& categorie,
                   int abonnes)
    : m_id(-1),
      m_codeUnique(codeUnique),
      m_nom(nom.trimmed()),
      m_prenom(prenom.trimmed()),
      m_dateAdhesion(dateAdhesion.isValid() ? dateAdhesion : QDate::currentDate()),
      m_categorie(categorie.trimmed()),
      m_abonnes(abonnes)
{}

void Createur::setId(int id)
{
    m_id = id;
}

int Createur::id() const
{
    return m_id;
}

bool Createur::ajouter()
{
    if (m_id < 0)
        m_id = nextId();

    QSqlQuery query;
    query.prepare(R"(INSERT INTO CREATEUR
                     (ID_CREATEUR, CODE_UNIQUE, NOM, PRENOM, DATE_ADHESION, CATEGORIE, ABONNEES)
                     VALUES (:id, :code, :nom, :prenom, :dateadh, :categorie, :abonnes))");
    query.bindValue(":id", m_id);
    query.bindValue(":code", m_codeUnique);
    query.bindValue(":nom", m_nom);
    query.bindValue(":prenom", m_prenom);
    query.bindValue(":dateadh", m_dateAdhesion);
    query.bindValue(":categorie", m_categorie);
    query.bindValue(":abonnes", m_abonnes);

    if (!query.exec()) {
        qWarning() << "Createur::ajouter error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Createur::modifier() const
{
    if (m_id < 0)
        return false;

    QSqlQuery query;
    query.prepare(R"(UPDATE CREATEUR
                     SET CODE_UNIQUE = :code,
                         NOM = :nom,
                         PRENOM = :prenom,
                         DATE_ADHESION = :dateadh,
                         CATEGORIE = :categorie,
                         ABONNEES = :abonnes
                     WHERE ID_CREATEUR = :id)");
    query.bindValue(":code", m_codeUnique);
    query.bindValue(":nom", m_nom);
    query.bindValue(":prenom", m_prenom);
    query.bindValue(":dateadh", m_dateAdhesion);
    query.bindValue(":categorie", m_categorie);
    query.bindValue(":abonnes", m_abonnes);
    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qWarning() << "Createur::modifier error:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool Createur::supprimer(int idCreateur)
{
    QSqlQuery query;
    query.prepare("DELETE FROM CREATEUR WHERE ID_CREATEUR = :id");
    query.bindValue(":id", idCreateur);

    if (!query.exec()) {
        qWarning() << "Createur::supprimer error:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

void Createur::afficher(QTableWidget* table,
                        const QString& searchText,
                        const QString& orderField)
{
    if (!table)
        return;

    table->clear();
    table->setColumnCount(CREATEUR_COLUMNS);
    table->setHorizontalHeaderLabels(QStringList()
                                     << QObject::tr("Code unique")
                                     << QObject::tr("Nom")
                                     << QObject::tr("Prénom")
                                     << QObject::tr("Date d'adhésion")
                                     << QObject::tr("Catégorie")
                                     << QObject::tr("Abonnés"));
    table->setRowCount(0);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->horizontalHeader()->setStretchLastSection(true);

    QString queryString = R"(SELECT ID_CREATEUR,
                                     CODE_UNIQUE,
                                     NOM,
                                     PRENOM,
                                     DATE_ADHESION,
                                     CATEGORIE,
                                     ABONNEES
                              FROM CREATEUR)";

    const QString trimmedSearch = searchText.trimmed();
    const bool hasSearch = !trimmedSearch.isEmpty();
    if (hasSearch) {
        queryString += R"( WHERE (
                               CAST(CODE_UNIQUE AS VARCHAR2(50)) LIKE :searchRaw OR
                               LOWER(NOM) LIKE :searchLower OR
                               LOWER(PRENOM) LIKE :searchLower OR
                               LOWER(CATEGORIE) LIKE :searchLower OR
                               CAST(ABONNEES AS VARCHAR2(50)) LIKE :searchRaw
                           ))";
    }

    QString orderColumn = "ID_CREATEUR";
    const QString orderUpper = orderField.trimmed().toUpper();
    if (orderUpper == "CODE_UNIQUE" || orderUpper == "CODE") {
        orderColumn = "CODE_UNIQUE";
    } else if (orderUpper == "NOM") {
        orderColumn = "NOM";
    } else if (orderUpper == "DATE_ADHESION" || orderUpper == "DATE") {
        orderColumn = "DATE_ADHESION";
    } else if (orderUpper == "ABONNEES") {
        orderColumn = "ABONNEES";
    }

    queryString += " ORDER BY " + orderColumn;

    QSqlQuery query;
    query.prepare(queryString);
    if (hasSearch) {
        query.bindValue(":searchRaw", "%" + trimmedSearch + "%");
        query.bindValue(":searchLower", "%" + trimmedSearch.toLower() + "%");
    }

    int row = 0;
    if (!query.exec()) {
        qWarning() << "Createur::afficher error:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        table->insertRow(row);

        const int id = query.value(0).toInt();
        const QString code = query.value(1).toString();
        const QString nom = query.value(2).toString();
        const QString prenom = query.value(3).toString();
        const QDate dateAdhesion = query.value(4).toDate();
        const QString categorie = query.value(5).toString();
        const QString abonnes = query.value(6).toString();

        auto *codeItem = new QTableWidgetItem(code);
        codeItem->setData(Qt::UserRole, id);
        table->setItem(row, 0, codeItem);

        table->setItem(row, 1, new QTableWidgetItem(nom));
        table->setItem(row, 2, new QTableWidgetItem(prenom));
        table->setItem(row, 3, new QTableWidgetItem(dateAdhesion.toString(DATE_DISPLAY_FORMAT)));
        table->setItem(row, 4, new QTableWidgetItem(categorie));
        table->setItem(row, 5, new QTableWidgetItem(abonnes));

        ++row;
    }
    table->resizeColumnsToContents();
}

bool Createur::codeUniqueExiste(int codeUnique, int excludeId)
{
    QSqlQuery query;
    QString statement = "SELECT 1 FROM CREATEUR WHERE CODE_UNIQUE = :code";
    if (excludeId >= 0)
        statement += " AND ID_CREATEUR <> :exclude";

    query.prepare(statement);
    query.bindValue(":code", codeUnique);
    if (excludeId >= 0)
        query.bindValue(":exclude", excludeId);

    if (!query.exec()) {
        qWarning() << "Createur::codeUniqueExiste error:" << query.lastError().text();
        return false;
    }
    return query.next();
}

bool Createur::codeUniqueValide(const QString& code)
{
    static const QRegularExpression re(R"(^\d{1,10}$)");
    return re.match(code).hasMatch();
}

bool Createur::nomPrenomValide(const QString& value)
{
    static const QRegularExpression re(R"(^[A-Za-zÀ-ÿ' -]{1,50}$)");
    return re.match(value).hasMatch();
}

bool Createur::categorieValide(const QString& value)
{
    static const QRegularExpression re(R"(^[A-Za-zÀ-ÿ0-9' -]{1,50}$)");
    return value.isEmpty() || re.match(value).hasMatch();
}

bool Createur::abonnesValide(const QString& value)
{
    static const QRegularExpression re(R"(^\d{1,10}$)");
    return re.match(value).hasMatch();
}

int Createur::nextId()
{
    QSqlQuery query("SELECT COALESCE(MAX(ID_CREATEUR), 0) + 1 FROM CREATEUR");
    if (query.next())
        return query.value(0).toInt();
    return 1;
}

