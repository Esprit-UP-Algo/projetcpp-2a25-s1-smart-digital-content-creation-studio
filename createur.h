#ifndef CREATEUR_H
#define CREATEUR_H

#include <QString>
#include <QDate>
#include <QTableWidget>

class Createur
{
public:
    Createur();
    Createur(int codeUnique,
             const QString& nom,
             const QString& prenom,
             const QDate& dateAdhesion,
             const QString& categorie,
             int abonnes);

    void setId(int id);
    int id() const;

    bool ajouter();
    bool modifier() const;
    static bool supprimer(int idCreateur);

    static void afficher(QTableWidget* table,
                         const QString& searchText = QString(),
                         const QString& orderField = QString());

    static bool codeUniqueExiste(int codeUnique, int excludeId = -1);
    static bool codeUniqueValide(const QString& code);
    static bool nomPrenomValide(const QString& value);
    static bool categorieValide(const QString& value);
    static bool abonnesValide(const QString& value);

private:
    static int nextId();

    int m_id;
    int m_codeUnique;
    QString m_nom;
    QString m_prenom;
    QDate m_dateAdhesion;
    QString m_categorie;
    int m_abonnes;
};

#endif // CREATEUR_H
