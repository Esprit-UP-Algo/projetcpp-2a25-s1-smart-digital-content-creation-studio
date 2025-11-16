    #ifndef EMPLOYEE_H
    #define EMPLOYEE_H

    #include <QString>
    #include <QSqlQueryModel>

    class employee
    {
    private:
        int id_employe;  // auto-increment dans la base
        int cin;
        int n_tel;
        QString nom;
        QString prenom;
        QString email;
        QString mdp;
        QString poste;
        double salaire;



    public:
        employee();
        employee(int cin, QString nom, QString prenom,
                 QString email, int n_tel, QString mdp,
                 QString poste, double salaire);

        // Getters
        int getCin() const;
        QString getNom() const;
        QString getPrenom() const;
        QString getEmail() const;
        int getTel() const;
        QString getMdp() const;
        QString getPoste() const;
        double getSalaire() const;

        // Setters
        void setCin(int);
        void setNom(QString);
        void setPrenom(QString);
        void setEmail(QString);
        void setTel(int);
        void setMdp(QString);
        void setPoste(QString);
        void setSalaire(double);

        // CRUD
        bool ajouter(int cin, QString nom, QString prenom,
                     QString email, int n_tel, QString mdp,
                     QString poste, double salaire);
        bool modifier(int id_employe, int newCin, QString newNom, QString newPrenom,
                      QString newEmail, int newTel, QString newMdp,
                      QString newPoste, double newSalaire);  // modification par ID
        QSqlQueryModel* afficher();
        bool supprimer(int cin);
        bool existe(int cin);

        // Affichages avancés
        QSqlQueryModel* afficherTrieParSalaire();
        QSqlQueryModel* afficherTrieParNom();
        QSqlQueryModel* rechercherParCin(const QString &cinPartiel);

    };

    #endif // EMPLOYEE_H
