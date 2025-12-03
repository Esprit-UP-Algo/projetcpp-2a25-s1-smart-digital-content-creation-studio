#include "employe.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlError>
#include <QObject>
#include <QBuffer>
#include <QByteArray>
#include <QImage>

// Constructeurs
employee::employee() {
}

employee::employee(int cin, QString nom, QString prenom,
                   QString email, int n_tel, QString mdp,
                   QString poste, QString role, double salaire) {
    this->cin = cin;
    this->nom = nom;
    this->prenom = prenom;
    this->email = email;
    this->n_tel = n_tel;
    this->mdp = mdp;
    this->poste = poste;
    this->role = role;
    this->salaire = salaire;
}

// Getters
int employee::getCin() const { return cin; }
QString employee::getNom() const { return nom; }
QString employee::getPrenom() const { return prenom; }
QString employee::getEmail() const { return email; }
int employee::getTel() const { return n_tel; }
QString employee::getMdp() const { return mdp; }
QString employee::getPoste() const { return poste; }
QString employee::getRole() const { return role; }
double employee::getSalaire() const { return salaire; }

// Setters
void employee::setCin(int c) { cin = c; }
void employee::setNom(QString n) { nom = n; }
void employee::setPrenom(QString p) { prenom = p; }
void employee::setEmail(QString e) { email = e; }
void employee::setTel(int t) { n_tel = t; }
void employee::setMdp(QString m) { mdp = m; }
void employee::setPoste(QString p) { poste = p; }
void employee::setRole(QString r) { role = r; }
void employee::setSalaire(double s) { salaire = s; }

// Ajouter un employé
bool employee::ajouter(int cin, QString nom, QString prenom,
                       QString email, int n_tel, QString mdp,
                       QString poste, QString role, double salaire)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Erreur base non ouverte:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO EMPLOYE (ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, ROLE, SALAIRE) "
                  "VALUES (EMPLOYE_SEQ.NEXTVAL, :cin, :nom, :prenom, :email, :n_tel, :mdp, :poste, :role, :salaire)");

    query.bindValue(":cin", cin);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":email", email);
    query.bindValue(":n_tel", n_tel);
    query.bindValue(":mdp", mdp);
    query.bindValue(":poste", poste);
    query.bindValue(":role", role);
    query.bindValue(":salaire", salaire);

    if (!query.exec()) {
        qDebug() << "Erreur SQL ajout:" << query.lastError().text();
        return false;
    }

    qDebug() << "Ajout effectué avec succès";
    return true;
}


// Modifier un employé par ID
bool employee::modifier(int id_employe, int newCin, QString newNom, QString newPrenom,
                        QString newEmail, int newTel, QString newMdp,
                        QString newPoste, QString newRole, double newSalaire)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE EMPLOYE SET CIN=:cin, NOM=:nom, PRENOM=:prenom, EMAIL=:email, "
                  "N_TEL=:n_tel, MDP=:mdp, POSTE=:poste, ROLE=:role, SALAIRE=:salaire "
                  "WHERE ID_EMPLOYE=:id");
    query.bindValue(":cin", newCin);
    query.bindValue(":nom", newNom);
    query.bindValue(":prenom", newPrenom);
    query.bindValue(":email", newEmail);
    query.bindValue(":n_tel", newTel);
    query.bindValue(":mdp", newMdp);
    query.bindValue(":poste", newPoste);
    query.bindValue(":role", newRole);
    query.bindValue(":salaire", newSalaire);
    query.bindValue(":id", id_employe);

    if (!query.exec()) {
        qDebug() << "Erreur SQL modification:" << query.lastError().text();
        return false;
    }

    qDebug() << "Modification réussie";
    return true;
}

// Supprimer un employé par CIN
bool employee::supprimer(int cin)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Erreur base non ouverte (supprimer):" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM EMPLOYE WHERE CIN = :cin");
    query.bindValue(":cin", cin);

    if (!query.exec()) {
        qDebug() << "Erreur SQL suppression:" << query.lastError().text();
        return false;
    }

    // query.numRowsAffected() peut être -1 selon les drivers, on ne l'utilise pas ici
    return true;
}

// Afficher tous les employés
QSqlQueryModel* employee::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, ROLE, SALAIRE FROM EMPLOYE");

    // Vérifie si la requête s’est bien exécutée
    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL afficher() :" << model->lastError().text();
    }

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID_EMPLOYE"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("CIN"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Prenom"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("N_TEL"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("MDP"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("POSTE"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("ROLE"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("SALAIRE"));

    return model;
}

QSqlQueryModel* employee::afficherTrieParSalaire()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, ROLE, SALAIRE FROM EMPLOYE ORDER BY SALAIRE DESC");

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL afficherTrieParSalaire() :" << model->lastError().text();
    }

    return model;
}

QSqlQueryModel* employee::afficherTrieParNom()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, ROLE, SALAIRE FROM EMPLOYE ORDER BY NOM ASC");

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL afficherTrieParNom() :" << model->lastError().text();
    }

    return model;
}

QSqlQueryModel* employee::rechercherParCin(const QString &cinPartiel)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    QString queryStr =
        "SELECT ID_EMPLOYE, CIN, NOM, PRENOM, EMAIL, N_TEL, MDP, POSTE, ROLE, SALAIRE "
        "FROM EMPLOYE "
        "WHERE TO_CHAR(CIN) LIKE '" + cinPartiel + "%'";

    model->setQuery(queryStr);

    if (model->lastError().isValid()) {
        qDebug() << "Erreur SQL rechercherParCin() :" << model->lastError().text();
    }

    return model;
}

bool employee::existe(int cin)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM EMPLOYE WHERE CIN = :cin");
    query.bindValue(":cin", cin);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

/*---------------------------------------------------
 * 📸 Face ID - Enregistrer une photo de visage
 *---------------------------------------------------*/
bool employee::saveFaceImage(const QString &email, const QImage &faceImage)
{
    if (faceImage.isNull()) {
        qDebug() << "Image vide, impossible de sauvegarder";
        return false;
    }

    // Convertir QImage en QByteArray
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    faceImage.save(&buffer, "PNG"); // Sauvegarder en PNG

    QSqlQuery query;
    query.prepare("UPDATE EMPLOYE SET FACE_IMAGE = :image WHERE EMAIL = :email");
    query.bindValue(":image", byteArray);
    query.bindValue(":email", email);

    if (!query.exec()) {
        qDebug() << "Erreur SQL saveFaceImage():" << query.lastError().text();
        return false;
    }

    qDebug() << "Photo de visage enregistrée pour:" << email;
    return true;
}

/*---------------------------------------------------
 * 📸 Face ID - Récupérer une photo de visage
 *---------------------------------------------------*/
QImage employee::getFaceImage(const QString &email)
{
    QSqlQuery query;
    query.prepare("SELECT FACE_IMAGE FROM EMPLOYE WHERE EMAIL = :email");
    query.bindValue(":email", email);

    if (!query.exec()) {
        qDebug() << "Erreur SQL getFaceImage():" << query.lastError().text();
        return QImage();
    }

    if (query.next()) {
        QByteArray byteArray = query.value(0).toByteArray();
        if (byteArray.isEmpty()) {
            qDebug() << "Aucune photo enregistrée pour:" << email;
            return QImage();
        }

        QImage image;
        image.loadFromData(byteArray, "PNG");
        return image;
    }

    return QImage();
}

/*---------------------------------------------------
 * 📸 Face ID - Authentification par reconnaissance faciale
 * (Version améliorée avec comparaison basique)
 *---------------------------------------------------*/
QString employee::authenticateByFace(const QImage &capturedImage)
{
    if (capturedImage.isNull()) {
        qDebug() << "Image capturée vide";
        return QString();
    }

    // Vérification basique : l'image doit avoir une taille raisonnable
    if (capturedImage.width() < 100 || capturedImage.height() < 100) {
        qDebug() << "Image trop petite pour contenir un visage";
        return QString();
    }

    // Récupérer tous les employés ayant une photo
    QSqlQuery query;
    query.prepare("SELECT EMAIL, FACE_IMAGE FROM EMPLOYE WHERE FACE_IMAGE IS NOT NULL");

    if (!query.exec()) {
        qDebug() << "Erreur SQL authenticateByFace():" << query.lastError().text();
        return QString();
    }

    // Parcourir tous les employés et comparer les images
    int bestMatchScore = 0;
    int secondBestScore = 0; // Pour vérifier qu'il y a un vrai écart avec le 2e meilleur
    QString bestMatchEmail;
    
    while (query.next()) {
        QString email = query.value(0).toString();
        QByteArray storedImageData = query.value(1).toByteArray();
        
        if (storedImageData.isEmpty()) {
            continue;
        }
        
        // Charger l'image stockée
        QImage storedImage;
        storedImage.loadFromData(storedImageData, "PNG");
        
        if (storedImage.isNull()) {
            continue;
        }
        
        // Comparaison basique : vérifier que les images ont une taille similaire
        // et comparer un échantillon de pixels
        int score = compareImages(capturedImage, storedImage);
        
        qDebug() << "Comparaison avec" << email << "- Score:" << score << "%";

        // Mise à jour des meilleurs scores
        if (score > bestMatchScore) {
            secondBestScore = bestMatchScore;
            bestMatchScore = score;
            bestMatchEmail = email;
        } else if (score > secondBestScore) {
            secondBestScore = score;
        }
    }

    // Conditions de reconnaissance :
    //  - score minimal modéré (par ex. >= 35 %)
    //  - et écart suffisant avec le 2e meilleur (par ex. au moins 5 %)
    if (!bestMatchEmail.isEmpty() && bestMatchScore >= 35 && (bestMatchScore - secondBestScore) >= 5) {
        qDebug() << "Face ID: Employé reconnu:" << bestMatchEmail << "avec score:" << bestMatchScore
                 << "(2e meilleur:" << secondBestScore << ")";
        return bestMatchEmail;
    }

    qDebug() << "Face ID: Aucun visage reconnu (meilleur score:" << bestMatchScore << "%)";
    return QString();
}

/*---------------------------------------------------
 * 📸 Comparaison basique d'images (sans OpenCV)
 * Retourne un score de similarité en pourcentage (0-100)
 *---------------------------------------------------*/
int employee::compareImages(const QImage &img1, const QImage &img2)
{
    // Redimensionner les images à la même taille pour la comparaison
    QImage img1Scaled = img1.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage img2Scaled = img2.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    
    int totalPixels = 100 * 100;
    int matchingPixels = 0;
    int threshold = 45; // Tolérance de différence de couleur (plus petit = comparaison plus stricte)
    
    // Comparer les pixels
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            QRgb pixel1 = img1Scaled.pixel(x, y);
            QRgb pixel2 = img2Scaled.pixel(x, y);
            
            int rDiff = qAbs(qRed(pixel1) - qRed(pixel2));
            int gDiff = qAbs(qGreen(pixel1) - qGreen(pixel2));
            int bDiff = qAbs(qBlue(pixel1) - qBlue(pixel2));
            
            // Si les couleurs sont similaires (dans le seuil de tolérance)
            if (rDiff < threshold && gDiff < threshold && bDiff < threshold) {
                matchingPixels++;
            }
        }
    }
    
    // Calculer le pourcentage de similarité
    int similarity = (matchingPixels * 100) / totalPixels;
    return similarity;
}
