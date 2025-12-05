#include "createur.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QBrush>
#include <QDebug>
#include <QPainter>
#include <QPdfWriter>
#include <QDateTime>
#include <QPageSize>
#include <QMarginsF>
#include <QPen>
#include <QFont>

namespace {
constexpr int CREATEUR_COLUMNS = 10;  // 9 -> 10
constexpr auto DATE_DISPLAY_FORMAT = "dd/MM/yyyy";
constexpr int SUBSCRIBER_THRESHOLD = 10000;
}

// ================== CONSTRUCTEURS ==================

Createur::Createur()
    : m_id(-1),
    m_codeUnique(0),
    m_nom(),
    m_prenom(),
    m_dateAdhesion(QDate::currentDate()),
    m_categorie(),
    m_statut(),
    m_abonnes(0),
    m_projets(0),
    m_t_projets(),
    m_t_sponsor()  // AJOUTÉ
{}

Createur::Createur(int codeUnique,
                   const QString &nom,
                   const QString &prenom,
                   const QDate &dateAdhesion,
                   const QString &categorie,
                   int abonnes,
                   int projets,
                   const QString &t_projets,
                   const QString &t_sponsor)  // AJOUTÉ
    : m_id(-1),
    m_codeUnique(codeUnique),
    m_nom(nom.trimmed()),
    m_prenom(prenom.trimmed()),
    m_dateAdhesion(dateAdhesion.isValid() ? dateAdhesion : QDate::currentDate()),
    m_categorie(categorie.trimmed()),
    m_statut(),
    m_abonnes(abonnes),
    m_projets(projets),
    m_t_projets(t_projets.trimmed()),
    m_t_sponsor(t_sponsor.trimmed())  // AJOUTÉ
{}

// ================== ACCESSEURS ==================

void Createur::setId(int id)
{
    m_id = id;
}

int Createur::id() const
{
    return m_id;
}

void Createur::setProjets(int projets)
{
    m_projets = projets;
}

int Createur::projets() const
{
    return m_projets;
}

void Createur::setTSponsor(const QString &t_sponsor)
{
    m_t_sponsor = t_sponsor.trimmed();
}

QString Createur::tSponsor() const
{
    return m_t_sponsor;
}

// ================== CRUD ==================

bool Createur::ajouter()
{
    if (m_id < 0)
        m_id = nextId();

    QSqlQuery query;
    query.prepare(R"(INSERT INTO CREATEUR
                     (ID_CREATEUR, CODE_UNIQUE, NOM, PRENOM, DATE_ADHESION,
                      CATEGORIE, ABONNEES, PROJETS, T_PROJETS, T_SPONSOR)
                     VALUES (:id, :code, :nom, :prenom, :dateadh,
                             :categorie, :abonnes, :projets, :t_projets, :t_sponsor))");

    query.bindValue(":id",        m_id);
    query.bindValue(":code",      m_codeUnique);
    query.bindValue(":nom",       m_nom);
    query.bindValue(":prenom",    m_prenom);
    query.bindValue(":dateadh",   m_dateAdhesion);
    query.bindValue(":categorie", m_categorie);
    query.bindValue(":abonnes",   m_abonnes);
    query.bindValue(":projets",   m_projets);
    query.bindValue(":t_projets",  m_t_projets);
    query.bindValue(":t_sponsor", m_t_sponsor);  // AJOUTÉ

    if (!query.exec()) {
        qDebug() << "Erreur INSERT CREATEUR:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Createur::modifier() const
{
    if (m_id < 0)
        return false;

    QSqlQuery query;
    query.prepare(R"(UPDATE CREATEUR SET
                     CODE_UNIQUE = :code,
                     NOM = :nom,
                     PRENOM = :prenom,
                     DATE_ADHESION = :dateadh,
                     CATEGORIE = :categorie,
                     ABONNEES = :abonnes,
                     PROJETS = :projets,
                     T_PROJETS = :t_projets,
                     T_SPONSOR = :t_sponsor
                     WHERE ID_CREATEUR = :id)");

    query.bindValue(":code",      m_codeUnique);
    query.bindValue(":nom",       m_nom);
    query.bindValue(":prenom",    m_prenom);
    query.bindValue(":dateadh",   m_dateAdhesion);
    query.bindValue(":categorie", m_categorie);
    query.bindValue(":abonnes",   m_abonnes);
    query.bindValue(":projets",   m_projets);
    query.bindValue(":t_projets",  m_t_projets);
    query.bindValue(":t_sponsor", m_t_sponsor);  // AJOUTÉ
    query.bindValue(":id",        m_id);

    if (!query.exec()) {
        qDebug() << "Erreur UPDATE CREATEUR:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Createur::supprimer(int idCreateur)
{
    QSqlQuery query;
    query.prepare("DELETE FROM CREATEUR WHERE ID_CREATEUR = :id");
    query.bindValue(":id", idCreateur);

    if (!query.exec()) {
        qDebug() << "Erreur DELETE CREATEUR:" << query.lastError().text();
        return false;
    }

    return true;
}

// ================== STATUT ==================

QString Createur::getStatusText(int abonnes)
{
    return abonnes >= SUBSCRIBER_THRESHOLD ? "Active" : "Inactive";
}

QColor Createur::getStatusColor(int abonnes)
{
    return abonnes >= SUBSCRIBER_THRESHOLD
               ? QColor(34, 197, 94)   // vert
               : QColor(239, 68, 68);  // rouge
}

QString Createur::getStatut() const
{
    return m_statut;
}

void Createur::calculerStatut()
{
    if (m_abonnes >= 10000)
        m_statut = "Actif";
    else
        m_statut = "Inactif";
}

// ================== AFFICHAGE TABLE ==================

void Createur::afficher(QTableWidget *table,
                        const QString &searchText,
                        const QString &orderField)
{
    Q_UNUSED(orderField);

    if (!table)
        return;

    table->clear();
    table->setColumnCount(CREATEUR_COLUMNS);  // 10 colonnes
    table->setHorizontalHeaderLabels(QStringList()
                                     << "Code unique"
                                     << "Nom"
                                     << "Prénom"
                                     << "Date d'adhésion"
                                     << "Catégorie"
                                     << "Abonnés"
                                     << "Statut"
                                     << "Projets"
                                     << "T_projets"
                                     << "T_sponsor");  // AJOUTÉ

    table->setRowCount(0);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QString queryStr =
        "SELECT ID_CREATEUR, CODE_UNIQUE, NOM, PRENOM, DATE_ADHESION, "
        "CATEGORIE, ABONNEES, PROJETS, T_PROJETS, T_SPONSOR FROM CREATEUR";  // AJOUTÉ T_SPONSOR

    if (!searchText.trimmed().isEmpty()) {
        QString search = searchText.trimmed();
        queryStr += " WHERE NOM LIKE '%" + search + "%' OR PRENOM LIKE '%" + search + "%'";
    }

    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qDebug() << "Erreur SELECT CREATEUR:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        table->insertRow(row);

        // 0 = ID_CREATEUR
        // 1 = CODE_UNIQUE
        // 2 = NOM
        // 3 = PRENOM
        // 4 = DATE_ADHESION
        // 5 = CATEGORIE
        // 6 = ABONNEES
        // 7 = PROJETS
        // 8 = T_PROJETS
        // 9 = T_SPONSOR

        int abonnes = query.value(6).toInt();

        // Col 0: code unique
        table->setItem(row, 0, new QTableWidgetItem(query.value(1).toString()));
        // Col 1: nom
        table->setItem(row, 1, new QTableWidgetItem(query.value(2).toString()));
        // Col 2: prenom
        table->setItem(row, 2, new QTableWidgetItem(query.value(3).toString()));
        // Col 3: date adhésion formatée
        table->setItem(row, 3,
                       new QTableWidgetItem(query.value(4).toDate()
                                                .toString(DATE_DISPLAY_FORMAT)));
        // Col 4: catégorie
        table->setItem(row, 4, new QTableWidgetItem(query.value(5).toString()));
        // Col 5: abonnés
        table->setItem(row, 5, new QTableWidgetItem(QString::number(abonnes)));

        // Col 6: statut coloré
        auto *statusItem = new QTableWidgetItem(getStatusText(abonnes));
        statusItem->setBackground(getStatusColor(abonnes));
        statusItem->setForeground(Qt::white);
        statusItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 6, statusItem);

        // Col 7: projets
        table->setItem(row, 7, new QTableWidgetItem(query.value(7).toString()));
        // Col 8: t_projets
        table->setItem(row, 8, new QTableWidgetItem(query.value(8).toString()));
        // Col 9: t_sponsor  // AJOUTÉ
        table->setItem(row, 9, new QTableWidgetItem(query.value(9).toString()));

        row++;
    }
}

// ================== VALIDATIONS / UTILS ==================

int Createur::nextId()
{
    QSqlQuery query("SELECT COALESCE(MAX(ID_CREATEUR), 0) + 1 FROM CREATEUR");
    if (!query.next())
        return 1;
    return query.value(0).toInt();
}

bool Createur::codeUniqueExiste(int codeUnique, int excludeId)
{
    QSqlQuery query;
    QString stmt = "SELECT 1 FROM CREATEUR WHERE CODE_UNIQUE = :code";
    if (excludeId >= 0)
        stmt += " AND ID_CREATEUR <> :exclude";

    query.prepare(stmt);
    query.bindValue(":code", codeUnique);
    if (excludeId >= 0)
        query.bindValue(":exclude", excludeId);

    if (!query.exec()) {
        qDebug() << "Erreur codeUniqueExiste:" << query.lastError().text();
        return false;
    }

    return query.next();
}

bool Createur::codeUniqueValide(const QString &code)
{
    return QRegularExpression(R"(^\d{1,10}$)").match(code).hasMatch();
}

bool Createur::nomPrenomValide(const QString &value)
{
    return QRegularExpression(R"(^[A-Za-zÀ-ÿ' -]{1,50}$)").match(value).hasMatch();
}

bool Createur::categorieValide(const QString &value)
{
    return value.isEmpty()
    || QRegularExpression(R"(^[A-Za-zÀ-ÿ0-9' -]{1,50}$)").match(value).hasMatch();
}

bool Createur::abonnesValide(const QString &value)
{
    return QRegularExpression(R"(^\d{1,10}$)").match(value).hasMatch();
}

// ================== PDF ==================

void Createur::genererPDFAffectation(const QString &filePath,
                                     int codeUnique,
                                     const QString &nom,
                                     const QString &prenom,
                                     int projets,
                                     const QString &t_projets,
                                     const QString &t_sponsor,  // AJOUTÉ
                                     const QDate &dateAdhesion) const
{
    QPdfWriter pdf(filePath);
    pdf.setPageSize(QPageSize::A4);
    pdf.setPageMargins(QMarginsF(20, 20, 20, 20));

    QPainter painter(&pdf);

    // Titre
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(100, 80, "HISTORIQUE DES AFFECTATIONS");

    // Ligne de séparation
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(100, 120, 500, 120);

    // Date de génération
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 9));
    QString dateGen = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
    painter.drawText(100, 160, "Date de génération : " + dateGen);

    // Infos créateur
    int yPos = 220;
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(100, yPos, "INFORMATIONS DU CRÉATEUR");

    yPos += 40;
    painter.setFont(QFont("Arial", 10));

    // Code unique
    painter.drawText(100, yPos, "Code unique :");
    painter.drawText(280, yPos, QString::number(codeUnique));
    painter.drawLine(100, yPos + 5, 500, yPos + 5);
    yPos += 35;

    // Nom
    painter.drawText(100, yPos, "Nom :");
    painter.drawText(280, yPos, nom);
    painter.drawLine(100, yPos + 5, 500, yPos + 5);
    yPos += 35;

    // Prénom
    painter.drawText(100, yPos, "Prénom :");
    painter.drawText(280, yPos, prenom);
    painter.drawLine(100, yPos + 5, 500, yPos + 5);
    yPos += 35;

    // Date d'adhésion
    painter.drawText(100, yPos, "Date d'adhésion :");
    painter.drawText(280, yPos, dateAdhesion.toString("dd/MM/yyyy"));
    painter.drawLine(100, yPos + 5, 500, yPos + 5);
    yPos += 50;

    // Section affectation
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(100, yPos, "AFFECTATION");

    yPos += 40;
    painter.setFont(QFont("Arial", 10));

    // Nombre de projets
    painter.drawText(100, yPos, "Nombre de projets :");
    painter.drawText(280, yPos, QString::number(projets));
    painter.drawLine(100, yPos + 5, 500, yPos + 5);
    yPos += 35;

    // T_projets
    painter.drawText(100, yPos, "T_projets :");
    painter.drawText(280, yPos, t_projets);
    painter.drawLine(100, yPos + 5, 500, yPos + 5);
    yPos += 35;

    // T_sponsor  // AJOUTÉ
    painter.drawText(100, yPos, "T_sponsor :");
    painter.drawText(280, yPos, t_sponsor);
    painter.drawLine(100, yPos + 5, 500, yPos + 5);
    yPos += 35;

    // Date d'affectation
    painter.drawText(100, yPos, "Date d'affectation :");
    painter.drawText(280, yPos,
                     QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    painter.drawLine(100, yPos + 5, 500, yPos + 5);

    painter.end();
}
