#include "employe.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QRegularExpression>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDebug>

Employe::Employe() {}

Employe::Employe(const QString& cin,
                 const QString& nom,
                 const QString& prenom,
                 const QString& email,
                 const QString& mode_passe,
                 const QString& n_gsm)
    : m_cin(cin.trimmed()),
    m_nom(nom.trimmed()),
    m_prenom(prenom.trimmed()),
    m_email(email.trimmed()),
    m_mode_passe(mode_passe),
    m_n_gsm(n_gsm.trimmed())
{}

// Controles de saisie
bool Employe::cinValide(const QString& cin)
{
    static const QRegularExpression re(R"(^\d{8}$)");
    return re.match(cin).hasMatch();
}

bool Employe::nomPrenomValide(const QString& s)
{
    static const QRegularExpression re(R"(^[A-Za-zÀ-ÿ' -]{1,50}$)");
    return re.match(s).hasMatch();
}

bool Employe::emailValide(const QString& email)
{
    // autorises: outlook, yahoo, gmail + .fr/.com
    static const QRegularExpression re(
        R"(^(?:[A-Za-z0-9._%+\-]+)@(outlook|yahoo|gmail)\.(fr|com)$)",
        QRegularExpression::CaseInsensitiveOption
        );
    return re.match(email).hasMatch();
}

bool Employe::telValide(const QString& tel)
{
    static const QRegularExpression re(R"(^\d{8}$)");
    return re.match(tel).hasMatch();
}

bool Employe::existeCin(const QString& cin)
{
    QSqlQuery q;
    q.prepare("SELECT 1 FROM Employe WHERE CIN = :cin");
    q.bindValue(":cin", cin);
    if (!q.exec()) {
        qWarning() << "existeCin error:" << q.lastError().text();
        return false;
    }
    return q.next();
}

bool Employe::ajouter() const
{
    QSqlQuery q;
    q.prepare(R"(INSERT INTO Employe
                 (CIN, nom, prenom, email, N_gsm, mode_passe)
                 VALUES (:cin, :nom, :prenom, :email, :n_gsm, :mode_passe))");
    q.bindValue(":cin", m_cin);
    q.bindValue(":nom", m_nom);
    q.bindValue(":prenom", m_prenom);
    q.bindValue(":email", m_email);
    q.bindValue(":n_gsm", m_n_gsm);
    q.bindValue(":mode_passe", m_mode_passe);

    if (!q.exec()) {
        qWarning() << "Employe::ajouter error:" << q.lastError().text();
        return false;
    }
    return true;
}

bool Employe::modifier(const QString& oldCin) const
{
    QSqlQuery q;
    q.prepare(R"(UPDATE Employe
                 SET CIN=:cin, nom=:nom, prenom=:prenom, email=:email, N_gsm=:n_gsm, mode_passe=:mode_passe
                 WHERE CIN=:oldcin)");
    q.bindValue(":cin", m_cin);
    q.bindValue(":nom", m_nom);
    q.bindValue(":prenom", m_prenom);
    q.bindValue(":email", m_email);
    q.bindValue(":n_gsm", m_n_gsm);
    q.bindValue(":mode_passe", m_mode_passe);
    q.bindValue(":oldcin", oldCin);

    if (!q.exec()) {
        qWarning() << "Employe::modifier error:" << q.lastError().text();
        return false;
    }
    return (q.numRowsAffected() > 0);
}

bool Employe::supprimer(const QString& cin)
{
    QSqlQuery q;
    q.prepare("DELETE FROM Employe WHERE CIN = :cin");
    q.bindValue(":cin", cin);

    if (!q.exec()) {
        qWarning() << "Employe::supprimer error:" << q.lastError().text();
        return false;
    }
    return (q.numRowsAffected() > 0);
}

void Employe::afficher(QTableWidget* table)
{
    if (!table) return;

    table->clear();
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(QStringList()
                                     << "CIN" << "Nom" << "Prenom" << "Email" << "N Tel" << "Mot de passe");
    table->setRowCount(0);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->horizontalHeader()->setStretchLastSection(true);

    QSqlQuery q(R"(SELECT CIN, nom, prenom, email, N_gsm, mode_passe
                   FROM Employe
                   ORDER BY id_employe)");
    int row = 0;
    while (q.next()) {
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(q.value(0).toString()));
        table->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));
        table->setItem(row, 2, new QTableWidgetItem(q.value(2).toString()));
        table->setItem(row, 3, new QTableWidgetItem(q.value(3).toString()));
        table->setItem(row, 4, new QTableWidgetItem(q.value(4).toString()));
        table->setItem(row, 5, new QTableWidgetItem(q.value(5).toString()));
        ++row;
    }
    table->resizeColumnsToContents();
}
