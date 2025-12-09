#ifndef CONTRAT_H
#define CONTRAT_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QTableWidget>

class Contrat
{
private:
    int id_contrat;
    QString type;
    int code_contrat;
    double prix;
    QDate date_signature;
    QDate echeance;
    int id_sponsor;
    int id_createur;

    // Nouveaux attributs pour la signature électronique
    QString signature_data;
    QDate date_signature_electronique;
    bool signature_valide;
    QString horodatage;
    QString statut_signature;

public:
    Contrat();
    Contrat(int id_contrat, const QString& type, int code_contrat, double prix,
            const QDate& date_signature, const QDate& echeance, int id_sponsor, int id_createur);

    bool ajouter();
    bool modifier();
    bool supprimer(int id);
    static void afficher(QTableWidget* table);
    static void rechercherParType(QTableWidget* table, const QString& type);
    static void trierParPrix(QTableWidget* tableWidget, bool croissant);
};

#endif // CONTRAT_H
