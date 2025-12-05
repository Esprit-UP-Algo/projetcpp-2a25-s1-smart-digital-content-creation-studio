#ifndef EMPLOYE_H
#define EMPLOYE_H

#include <QString>
#include <QTableWidget>

class Employe
{
public:
    Employe();
    Employe(const QString& cin,
            const QString& nom,
            const QString& prenom,
            const QString& email,
            const QString& mode_passe,
            const QString& n_gsm);

    bool ajouter() const;
    bool modifier(const QString& oldCin) const;
    static bool supprimer(const QString& cin);

    static void afficher(QTableWidget* table);
    static bool existeCin(const QString& cin);

    // Controles de saisie
    static bool cinValide(const QString& cin);          // exactement 8 chiffres
    static bool nomPrenomValide(const QString& s);      // lettres + accents + (espace ' -)
    static bool emailValide(const QString& email);      // domaines: outlook/yahoo/gmail + TLD fr/com
    static bool telValide(const QString& tel);          // exactement 8 chiffres

private:
    QString m_cin, m_nom, m_prenom, m_email, m_mode_passe, m_n_gsm;
};

#endif // EMPLOYE_H
