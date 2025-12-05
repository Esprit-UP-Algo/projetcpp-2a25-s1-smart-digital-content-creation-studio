#ifndef CREATEUR_H
#define CREATEUR_H

#include <QString>
#include <QDate>
#include <QTableWidget>
#include <QColor>

class Createur
{
public:
    Createur();
    Createur(int codeUnique,
             const QString &nom,
             const QString &prenom,
             const QDate &dateAdhesion,
             const QString &categorie,
             int abonnes,
             int projets,
             const QString &t_projets = QString(),
             const QString &t_sponsor = QString());  // AJOUTÉ

    void setId(int id);
    int id() const;

    void setProjets(int projets);
    int projets() const;

    void setTSponsor(const QString &t_sponsor);  // AJOUTÉ
    QString tSponsor() const;                     // AJOUTÉ

    bool ajouter();
    bool modifier() const;
    static bool supprimer(int idCreateur);

    static void afficher(QTableWidget *table,
                         const QString &searchText = QString(),
                         const QString &orderField = QString());

    static bool codeUniqueExiste(int codeUnique, int excludeId = -1);
    static bool codeUniqueValide(const QString &code);
    static bool nomPrenomValide(const QString &value);
    static bool categorieValide(const QString &value);
    static bool abonnesValide(const QString &value);

    static QString getStatusText(int abonnes);
    static QColor getStatusColor(int abonnes);

    QString getStatut() const;
    void calculerStatut();

    void genererPDFAffectation(const QString &filePath,
                               int codeUnique,
                               const QString &nom,
                               const QString &prenom,
                               int projets,
                               const QString &t_projets,
                               const QString &t_sponsor,  // AJOUTÉ
                               const QDate &dateAdhesion) const;

private:
    static int nextId();

    int m_id;
    int m_codeUnique;
    QString m_nom;
    QString m_prenom;
    QDate m_dateAdhesion;
    QString m_categorie;
    QString m_statut;
    int m_abonnes;
    int m_projets;
    QString m_t_projets;
    QString m_t_sponsor;  // AJOUTÉ
};

#endif // CREATEUR_H
