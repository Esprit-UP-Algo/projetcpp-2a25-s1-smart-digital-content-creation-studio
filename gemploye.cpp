#include "gemploye.h"
#include "ui_gemploye.h"
#include "employe.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColor>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>
#include <QtCharts/QLegendMarker>
#include <QTableView>
#include <QStandardItemModel>
#include <QDebug>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QVideoWidget>
#include <QDialog>
#include <QPushButton>
#include <QProcess>
#include <QDate>
#include "logindialog.h"

#include "gprojet.h"

Gemploye::Gemploye(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gemploye)
{
    ui->setupUi(this);

    // S'assurer que l'onglet "statistique" (tab_4) utilise un layout
    if (!ui->tab_4->layout()) {
        auto *statsLayout = new QVBoxLayout(ui->tab_4);
        statsLayout->setContentsMargins(20, 20, 20, 20);
        statsLayout->setSpacing(20);
        statsLayout->addWidget(ui->widget_6);
    }

    // Permettre à widget_6 de prendre toute la place disponible dans l'onglet
    ui->widget_6->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Configuration du tableau
    ui->tableemp->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableemp->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableemp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableemp->horizontalHeader()->setStretchLastSection(true);
    ui->tableemp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Logo
    QPixmap logo("C:/Users/LOQ/Documents/Gemploye/images/logo.png");
    ui->label_7->setPixmap(logo.scaled(ui->label_7->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // ✅ Chargement automatique au démarrage
    ui->tableemp->setModel(e_global.afficher());
    ui->tableemp->hideColumn(0); // 👈 Cache la colonne ID_EMPLOYE

    // Contrôle de saisie (regex + placeholders)
    QRegularExpression cinRegex("^[0-9]{6,12}$");  // CIN : chiffres 6-12
    QRegularExpression nameRegex("^[A-Za-zÀ-ÖØ-öø-ÿ\\s\\-']{2,50}$"); // nom/prénom
    QRegularExpression emailRegex("^[\\w\\.\\-]+@[\\w\\-]+\\.[A-Za-z]{2,6}$"); // email basique
    QRegularExpression telRegex("^\\+?[0-9]{7,15}$"); // téléphone
    QRegularExpression roleRegex("^[A-Za-zÀ-ÖØ-öø-ÿ\\s\\-']{2,50}$"); // rôle

    ui->linecin->setValidator(new QRegularExpressionValidator(cinRegex, this));
    ui->linenom->setValidator(new QRegularExpressionValidator(nameRegex, this));
    ui->lineprenom->setValidator(new QRegularExpressionValidator(nameRegex, this));
    ui->linentel_2->setValidator(new QRegularExpressionValidator(nameRegex, this)); // poste
    ui->lineemail->setPlaceholderText("exemple@domaine.com");
    ui->linentel->setValidator(new QRegularExpressionValidator(telRegex, this));
    ui->linemdp->setPlaceholderText("8+ caractères, maj, min, chiffre, symbole");
    ui->linemdp->setEchoMode(QLineEdit::Password);
    ui->linentel_4->setValidator(new QRegularExpressionValidator(roleRegex, this)); // rôle

    // Validateur pour le salaire : nombre décimal positif avec 2 décimales max
    auto *salaryValidator = new QDoubleValidator(0, 1e9, 2, this);
    salaryValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->linentel_3->setValidator(salaryValidator);
}

Gemploye::~Gemploye()
{
    delete ui;
}

// Appelée après la connexion à la base de données depuis main.cpp
// Pour l'instant, toute l'initialisation (CRUD, stats, etc.) est déjà
// faite dans le constructeur, donc cette méthode ne fait rien de plus.
void Gemploye::initAfterConnect()
{
    // Point d'extension éventuel si tu veux ajouter une logique spéciale
    // après la connexion à la base.
}

/*---------------------------------------------------
 * 🔐 Configuration des permissions selon le rôle
 *---------------------------------------------------*/
void Gemploye::configurePermissions(const QString &role)
{
    // Par défaut, tous les boutons sont désactivés
    ui->Employ->setEnabled(false);
    ui->Employ_4->setEnabled(false);
    ui->Employ_2->setEnabled(false);
    ui->Employ_6->setEnabled(false);
    ui->Employ_3->setEnabled(false);
    ui->Employ_5->setEnabled(false);

    // Activation selon le rôle (insensible à la casse)
    QString roleLower = role.toLower().trimmed();
    
    if (roleLower == "employe" || roleLower == "employé") {
        ui->Employ->setEnabled(true);
        ui->stackedWidget->setCurrentIndex(0);
    }
    else if (roleLower == "projet") {
        ui->Employ_4->setEnabled(true);
        // Afficher directement la page projet complète intégrée (Gprojet)
        on_Employ_4_clicked();
    }
    else if (roleLower == "contrat") {
        ui->Employ_2->setEnabled(true);
        ui->stackedWidget->setCurrentIndex(2);
    }
    else if (roleLower == "sponsor") {
        ui->Employ_6->setEnabled(true);
        ui->stackedWidget->setCurrentIndex(3);
    }
    else if (roleLower == "createur de contenu" || roleLower == "créateur de contenu") {
        ui->Employ_3->setEnabled(true);
        ui->stackedWidget->setCurrentIndex(4);
    }
    else if (roleLower == "materiel" || roleLower == "matériel") {
        ui->Employ_5->setEnabled(true);
        ui->stackedWidget->setCurrentIndex(5);
    }
    else {
        // Si le rôle n'est pas reconnu, donner accès à tout (admin ou autre)
        ui->Employ->setEnabled(true);
        ui->Employ_4->setEnabled(true);
        ui->Employ_2->setEnabled(true);
        ui->Employ_6->setEnabled(true);
        ui->Employ_3->setEnabled(true);
        ui->Employ_5->setEnabled(true);
    }
}

void Gemploye::on_tabWidget_currentChanged(int index)
{
    // Onglet 1 = "statistique" dans tabWidget
    if (index == 1) {
        afficherStatistiques();
    }
}

/*---------------------------------------------------
 * 🧩 Validation des champs
 *---------------------------------------------------*/
bool Gemploye::validerChamps(QString &erreur)
{
    QString cin = ui->linecin->text().trimmed();
    QString nom = ui->linenom->text().trimmed();
    QString prenom = ui->lineprenom->text().trimmed();
    QString email = ui->lineemail->text().trimmed();
    QString tel = ui->linentel->text().trimmed();
    QString mdp = ui->linemdp->text();
    QString poste = ui->linentel_2->text().trimmed();
    QString salaireStr = ui->linentel_3->text().trimmed();
    QString role = ui->linentel_4->text().trimmed();

    // Vérifier si un champ est vide
    if (cin.isEmpty() || nom.isEmpty() || prenom.isEmpty() || email.isEmpty() ||
        tel.isEmpty() || mdp.isEmpty() || poste.isEmpty() || salaireStr.isEmpty() ||
        role.isEmpty()) {
        erreur = "Tous les champs doivent être remplis.";
        return false;
    }

    // Expressions régulières
    QRegularExpression cinRegex("^[0-9]{6,12}$");
    QRegularExpression nameRegex("^[A-Za-zÀ-ÖØ-öø-ÿ\\s\\-']{2,50}$");
    QRegularExpression emailRegex("^[\\w\\.\\-]+@[\\w\\-]+\\.[A-Za-z]{2,6}$");
    QRegularExpression telRegex("^\\+?[0-9]{7,15}$");
    QRegularExpression mdpRegex("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)(?=.*[^A-Za-z0-9]).{8,}$");
    QRegularExpression roleRegex("^[A-Za-zÀ-ÖØ-öø-ÿ\\s\\-']{2,50}$");

    if (!cinRegex.match(cin).hasMatch()) {
        erreur = "CIN invalide (6 à 12 chiffres).";
        return false;
    }
    if (!nameRegex.match(nom).hasMatch()) {
        erreur = "Nom invalide (lettres uniquement, 2 à 50 caractères).";
        return false;
    }
    if (!nameRegex.match(prenom).hasMatch()) {
        erreur = "Prénom invalide (lettres uniquement, 2 à 50 caractères).";
        return false;
    }
    if (!emailRegex.match(email).hasMatch()) {
        erreur = "Format d'email invalide (ex: exemple@domaine.com).";
        return false;
    }
    if (!telRegex.match(tel).hasMatch()) {
        erreur = "Numéro de téléphone invalide (7 à 15 chiffres).";
        return false;
    }
    if (!roleRegex.match(role).hasMatch()) {
        erreur = "Rôle invalide (lettres uniquement, 2 à 50 caractères).";
        return false;
    }
    bool okSalaire = false;
    double salaire = salaireStr.toDouble(&okSalaire);
    if (!okSalaire || salaire <= 0) {
        erreur = "Salaire invalide (nombre positif).";
        return false;
    }
    if (!mdpRegex.match(mdp).hasMatch()) {
        erreur = "Mot de passe faible : 8+ caractères avec majuscules, minuscules, chiffres et symbole.";
        return false;
    }

    return true;
}

/*---------------------------------------------------
 * 🧩 Navigation entre pages du stackedWidget / ouverture gestion projet
 *---------------------------------------------------*/
void Gemploye::on_Employ_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Gemploye::on_Employ_4_clicked()
{
    // Affiche la page "Projet" dans le stackedWidget principal
    ui->stackedWidget->setCurrentIndex(1);

    // Embarque la fenêtre de gestion de projet (Gprojet) à l'intérieur de cette page
    if (!m_projetWindow)
    {
        // Masquer les anciens widgets de la page projet (s'ils existent encore)
        const auto children = ui->page_2->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
        for (QWidget *w : children)
        {
            w->hide();
        }

        // Créer Gprojet comme widget enfant de page_2
        m_projetWindow = new Gprojet(ui->page_2);
        m_projetWindow->initAfterConnect();
        m_projetWindow->setWindowFlags(Qt::Widget); // intégré, plus de fenêtre séparée

        auto *layout = new QVBoxLayout(ui->page_2);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(m_projetWindow);
    }
}
void Gemploye::on_Employ_2_clicked() { ui->stackedWidget->setCurrentIndex(2); }
void Gemploye::on_Employ_6_clicked() { ui->stackedWidget->setCurrentIndex(3); }
void Gemploye::on_Employ_3_clicked() { ui->stackedWidget->setCurrentIndex(4); }
void Gemploye::on_Employ_5_clicked() { ui->stackedWidget->setCurrentIndex(5); }

/*---------------------------------------------------
 * ➕ Ajouter un employé
 *---------------------------------------------------*/
void Gemploye::on_ajouter_clicked()
{
    QString erreur;
    if (!validerChamps(erreur)) {
        QMessageBox::warning(this, "Erreur de saisie", erreur);
        return;
    }

    int cin = ui->linecin->text().toInt();
    QString nom = ui->linenom->text();
    QString prenom = ui->lineprenom->text();
    QString email = ui->lineemail->text();
    int n_tel = ui->linentel->text().toInt();
    QString mdp = ui->linemdp->text();
    QString poste = ui->linentel_2->text();
    double salaire = ui->linentel_3->text().toDouble();
    QString role = ui->linentel_4->text();

    employee e;

    // 🚫 Vérifie si le CIN existe déjà
    if (e.existe(cin)) {
        QMessageBox::warning(this, "Erreur", "Un employé avec ce CIN existe déjà !");
        return;
    }

    // ✅ Si non, on l'ajoute
    if (e.ajouter(cin, nom, prenom, email, n_tel, mdp, poste, role, salaire)) {
        // Sauvegarder la photo Face ID si elle a été capturée
        if (!m_capturedFaceImage.isNull()) {
            if (employee::saveFaceImage(email, m_capturedFaceImage)) {
                qDebug() << "Photo Face ID enregistrée pour:" << email;
            } else {
                qDebug() << "Erreur lors de l'enregistrement de la photo Face ID";
            }
            m_capturedFaceImage = QImage(); // Réinitialiser
        }
        
        ui->tableemp->setModel(e_global.afficher());
        ui->tableemp->hideColumn(0); // 👈 Cache la colonne ID_EMPLOYE
        QMessageBox::information(this, "Succès", "Employé ajouté avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout. Vérifiez les données ou le CIN.");
    }
}


/*---------------------------------------------------
 * ✏️ Modifier un employé
 *---------------------------------------------------*/
void Gemploye::on_modifier_clicked()
{
    if (selectedId == 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un employé à modifier.");
        return;
    }

    QString erreur;
    if (!validerChamps(erreur)) {
        QMessageBox::warning(this, "Erreur de saisie", erreur);
        return;
    }

    int cin = ui->linecin->text().toInt();
    QString nom = ui->linenom->text();
    QString prenom = ui->lineprenom->text();
    QString email = ui->lineemail->text();
    int n_tel = ui->linentel->text().toInt();
    QString mdp = ui->linemdp->text();
    QString poste = ui->linentel_2->text();
    double salaire = ui->linentel_3->text().toDouble();
    QString role = ui->linentel_4->text();

    employee e;

    // 🚫 Vérifie si le CIN appartient à un autre employé
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT ID_EMPLOYE FROM EMPLOYE WHERE CIN = :cin");
    checkQuery.bindValue(":cin", cin);

    if (checkQuery.exec() && checkQuery.next()) {
        int existingId = checkQuery.value(0).toInt();
        if (existingId != selectedId) {
            QMessageBox::warning(this, "Erreur", "Ce CIN est déjà utilisé par un autre employé !");
            return;
        }
    }

    // ✅ Si tout est bon, on modifie
    if (e_global.modifier(selectedId, cin, nom, prenom, email, n_tel, mdp, poste, role, salaire)) {
        // Sauvegarder la photo Face ID si elle a été capturée
        if (!m_capturedFaceImage.isNull()) {
            if (employee::saveFaceImage(email, m_capturedFaceImage)) {
                qDebug() << "Photo Face ID mise à jour pour:" << email;
            } else {
                qDebug() << "Erreur lors de la mise à jour de la photo Face ID";
            }
            m_capturedFaceImage = QImage(); // Réinitialiser
        }
        
        ui->tableemp->setModel(e_global.afficher());
        ui->tableemp->hideColumn(0); // Cache ID_EMPLOYE
        QMessageBox::information(this, "Succès", "Modification effectuée avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification. Vérifiez les informations.");
    }
}


/*---------------------------------------------------
 * 📋 Sélection d'une ligne du tableau
 *---------------------------------------------------*/
void Gemploye::on_tableemp_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    QAbstractItemModel *model = ui->tableemp->model();

    selectedId = model->data(model->index(row, 0)).toInt(); // ID_EMPLOYE
    ui->linecin->setText(model->data(model->index(row, 1)).toString());
    ui->linenom->setText(model->data(model->index(row, 2)).toString());
    ui->lineprenom->setText(model->data(model->index(row, 3)).toString());
    ui->lineemail->setText(model->data(model->index(row, 4)).toString());
    ui->linentel->setText(model->data(model->index(row, 5)).toString());
    ui->linemdp->setText(model->data(model->index(row, 6)).toString());
    ui->linentel_2->setText(model->data(model->index(row, 7)).toString()); // POSTE
    ui->linentel_4->setText(model->data(model->index(row, 8)).toString()); // ROLE
    ui->linentel_3->setText(model->data(model->index(row, 9)).toString()); // SALAIRE
}

/*---------------------------------------------------
 * ❌ Supprimer un employé
 *---------------------------------------------------*/
void Gemploye::on_supprimer_clicked()
{
    // On utilise le champ de saisie dédié à la suppression (en bas, à côté du bouton Supprimer)
    QString cinStr = ui->lineEdit_14->text().trimmed();
    int cin = cinStr.toInt();

    if (cinStr.isEmpty() || cin == 0) {
        QMessageBox::warning(this, "Attention", "Veuillez entrer un numéro de CIN valide !");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation",
        "Voulez-vous vraiment supprimer cet employé ?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // Vérifier d'abord que ce CIN existe
        if (!e_global.existe(cin)) {
            QMessageBox::warning(this, "Erreur", "Aucun employé trouvé avec ce CIN.");
            return;
        }

        if (e_global.supprimer(cin)) {
            ui->tableemp->setModel(e_global.afficher());
            ui->tableemp->hideColumn(0); // 👈 Cache la colonne ID_EMPLOYE
            QMessageBox::information(this, "Succès", "Employé supprimé avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur", "Suppression échouée. Vérifiez le CIN.");
        }
    }
}

void Gemploye::on_refrech_clicked()
{
    ui->tableemp->setModel(e_global.afficher());
    ui->tableemp->hideColumn(0);
}

void Gemploye::on_actualiser_clicked()
{
    QSqlQueryModel *model = e_global.afficher();
    ui->tableemp->setModel(model);
    ui->tableemp->hideColumn(0);
}

void Gemploye::on_lineEdit_13_textChanged(const QString &arg1)
{
    if (arg1.trimmed().isEmpty()) {
        ui->tableemp->setModel(e_global.afficher());
        ui->tableemp->hideColumn(0);
        return;
    }

    QSqlQueryModel *model = e_global.rechercherParCin(arg1.trimmed());
    ui->tableemp->setModel(model);
    ui->tableemp->hideColumn(0);
}

void Gemploye::on_comboBox_currentIndexChanged(int index)
{
    QSqlQueryModel *model = nullptr;

    if (index == 0) {
        model = e_global.afficherTrieParSalaire();
    } else if (index == 1) {
        model = e_global.afficherTrieParNom();
    } else {
        model = e_global.afficher();
    }

    ui->tableemp->setModel(model);
    ui->tableemp->hideColumn(0);
}

void Gemploye::on_pushButton_5_clicked()
{
    if (selectedId == 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un employé dans le tableau avant d'exporter l'attestation.");
        return;
    }

    QString nom = ui->linenom->text().trimmed();
    QString prenom = ui->lineprenom->text().trimmed();
    QString cin = ui->linecin->text().trimmed();
    QString poste = ui->linentel_2->text().trimmed();
    QString salaire = ui->linentel_3->text().trimmed();

    if (nom.isEmpty() || prenom.isEmpty() || cin.isEmpty() || poste.isEmpty()) {
        QMessageBox::warning(this, "Attention", "Les informations de l'employé sélectionné sont incomplètes.");
        return;
    }

    QString defaultFileName = QString("Attestation_%1_%2.pdf").arg(nom).arg(prenom);
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter attestation en PDF", defaultFileName, "Fichiers PDF (*.pdf)");
    if (fileName.isEmpty())
        return;

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(300);

    QPainter painter(&pdfWriter);
    if (!painter.isActive()) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier PDF.");
        return;
    }

    QFont titleFont("Times New Roman", 18, QFont::Bold);
    painter.setFont(titleFont);

    int topMargin = 200;
    QRect titleRect(0, topMargin, pdfWriter.width(), 200);
    painter.drawText(titleRect, Qt::AlignCenter, QStringLiteral("ATTESTATION DE TRAVAIL"));

    QFont bodyFont("Times New Roman", 12);
    painter.setFont(bodyFont);

    int leftMargin = 300;
    int lineHeight = 200;
    int y = topMargin + 400;

    QString line1 = tr("Je soussigné(e), représentant légal de l'entreprise, atteste que :");
    QString line2 = tr("%1 %2, titulaire de la CIN n° %3,").arg(prenom, nom, cin);
    QString line3 = tr("est actuellement employé(e) en qualité de %1.").arg(poste);
    QString line4;
    if (!salaire.isEmpty()) {
        line4 = tr("Il/Elle perçoit un salaire brut mensuel de %1.").arg(salaire);
    }
    QString line5 = tr("La présente attestation est délivrée à la demande de l'intéressé(e)");
    QString line6 = tr("pour servir et valoir ce que de droit.");
    QString line7 = tr("Fait à ____________________, le %1.").arg(QDate::currentDate().toString("dd/MM/yyyy"));

    QRect textRect(leftMargin, y, pdfWriter.width() - 2 * leftMargin, lineHeight);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, line1);
    y += lineHeight;
    textRect.moveTop(y);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, line2);
    y += lineHeight;
    textRect.moveTop(y);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, line3);
    if (!line4.isEmpty()) {
        y += lineHeight;
        textRect.moveTop(y);
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, line4);
    }
    y += 2 * lineHeight;
    textRect.moveTop(y);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, line5);
    y += lineHeight;
    textRect.moveTop(y);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, line6);
    y += 2 * lineHeight;
    textRect.moveTop(y);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, line7);

    y += 3 * lineHeight;
    textRect.moveTop(y);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, tr("Signature et cachet"));

    painter.end();

    QMessageBox::information(this, "Succès", "Attestation de travail exportée avec succès.");
}

void Gemploye::afficherStatistiques()
{
    // Nettoyer tout contenu précédent du widget de stats
    QLayout *oldLayout = ui->widget_6->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget())
                delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    // Layout principal : vertical, avec une rangée horizontale pour les 2 graphiques
    auto *layout = new QVBoxLayout(ui->widget_6);
    layout->setContentsMargins(40, 30, 40, 30);
    layout->setSpacing(30);
    ui->widget_6->setLayout(layout);

    QHBoxLayout *chartsRow = new QHBoxLayout();
    chartsRow->setSpacing(40);
    layout->addLayout(chartsRow);

    // 1) Répartition par poste
    QSqlQueryModel *modelPoste = new QSqlQueryModel(ui->widget_6);
    modelPoste->setQuery(
        "SELECT POSTE, COUNT(*) AS NB_EMPLOYES, ROUND(AVG(SALAIRE), 2) AS SALAIRE_MOYEN "
        "FROM EMPLOYE "
        "GROUP BY POSTE "
        "ORDER BY COUNT(*) DESC");

    qDebug() << "[STAT] modelPoste rowCount =" << modelPoste->rowCount();

    // En-têtes lisibles pour le tableau des postes
    modelPoste->setHeaderData(0, Qt::Horizontal, tr("Poste"));
    modelPoste->setHeaderData(1, Qt::Horizontal, tr("Nb employés"));
    modelPoste->setHeaderData(2, Qt::Horizontal, tr("Salaire moyen"));

    if (modelPoste->rowCount() == 0) {
        QLabel *label = new QLabel(tr("Aucune donnée pour les statistiques."), ui->widget_6);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        return;
    }

    QPieSeries *seriesPoste = new QPieSeries();
    seriesPoste->setPieSize(0.7);
    seriesPoste->setHoleSize(0.45);
    QStringList postesLabels;
    for (int i = 0; i < modelPoste->rowCount(); ++i) {
        QString poste = modelPoste->data(modelPoste->index(i, 0)).toString();
        int count = modelPoste->data(modelPoste->index(i, 1)).toInt();
        // double salaireMoyen = modelPoste->data(modelPoste->index(i, 2)).toDouble(); // Non utilisé pour l'instant

        // Conserver le nom du poste pour la légende
        postesLabels << poste;
        seriesPoste->append(poste, count);
    }

    qreal totalPoste = seriesPoste->sum();
    seriesPoste->setLabelsVisible(true);

    // Couleurs dynamiques : une teinte différente par poste (pas de répétition dans le même graphique)
    int sliceCountPoste = seriesPoste->count();
    for (int i = 0; i < sliceCountPoste; ++i) {
        QPieSlice *slice = seriesPoste->slices().at(i);

        // Répartition des teintes sur le cercle HSV
        qreal hue = static_cast<qreal>(i) / qMax(1, sliceCountPoste); // 0.0 -> 1.0
        QColor color;
        color.setHsvF(hue, 0.75, 0.95); // saturation et valeur élevées pour des couleurs vives

        slice->setBrush(color);

        double pourcentage = (totalPoste > 0.0) ? (slice->value() * 100.0 / totalPoste) : 0.0;
        slice->setLabel(QString("%1%")
                        .arg(pourcentage, 0, 'f', 1));
        slice->setLabelColor(Qt::black);
        slice->setLabelPosition(QPieSlice::LabelOutside);
    }

    QChart *chartPoste = new QChart();
    chartPoste->addSeries(seriesPoste);
    chartPoste->setTitle(tr("Répartition des employés par poste"));
    QFont titrePosteFont = chartPoste->titleFont();
    titrePosteFont.setBold(true);
    chartPoste->setTitleFont(titrePosteFont);
    chartPoste->legend()->setAlignment(Qt::AlignRight);
    chartPoste->legend()->setFont(QFont("Segoe UI", 9));
    // Forcer la légende à n'afficher que les noms de poste
    const auto markersPoste = chartPoste->legend()->markers(seriesPoste);
    for (int i = 0; i < markersPoste.size() && i < postesLabels.size(); ++i) {
        if (markersPoste.at(i))
            markersPoste.at(i)->setLabel(postesLabels.at(i));
    }
    chartPoste->setBackgroundBrush(Qt::white);
    chartPoste->setBackgroundPen(Qt::NoPen);
    chartPoste->setMargins(QMargins(20, 40, 20, 40));

    QChartView *chartViewPoste = new QChartView(chartPoste, ui->widget_6);
    chartViewPoste->setRenderHint(QPainter::Antialiasing);
    chartViewPoste->setStyleSheet("background-color: white; border: 2px solid #5e35b1; border-radius: 8px;");
    chartViewPoste->setMinimumHeight(360);
    chartViewPoste->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartsRow->addWidget(chartViewPoste, 1);

    // 2) Répartition par tranches de salaire
    QSqlQueryModel *modelSalaire = new QSqlQueryModel(ui->widget_6);
    modelSalaire->setQuery(
        "SELECT "
        "  COALESCE(CASE "
        "    WHEN SALAIRE < 1000 THEN '< 1000' "
        "    WHEN SALAIRE < 2000 THEN '1000 - 1999' "
        "    WHEN SALAIRE < 3000 THEN '2000 - 2999' "
        "    ELSE '>= 3000' "
        "  END, '< 1000') AS TRANCHE, "
        "  COUNT(*) AS NB_EMPLOYES "
        "FROM EMPLOYE "
        "GROUP BY COALESCE(CASE "
        "    WHEN SALAIRE < 1000 THEN '< 1000' "
        "    WHEN SALAIRE < 2000 THEN '1000 - 1999' "
        "    WHEN SALAIRE < 3000 THEN '2000 - 2999' "
        "    ELSE '>= 3000' "
        "  END, '< 1000') "
        "ORDER BY TRANCHE");

    qDebug() << "[STAT] modelSalaire rowCount =" << modelSalaire->rowCount();

    // En-têtes lisibles pour le tableau des tranches de salaire
    modelSalaire->setHeaderData(0, Qt::Horizontal, tr("Tranche"));
    modelSalaire->setHeaderData(1, Qt::Horizontal, tr("Nb employés"));

    // Construire une série avec les 4 tranches fixes pour garantir la présence dans la légende
    QMap<QString, int> compteParTranche;
    // Initialiser les tranches avec 0
    compteParTranche["<1000"] = 0;
    compteParTranche["1000 - 1999"] = 0;
    compteParTranche["2000 - 2999"] = 0;
    compteParTranche[">= 3000"] = 0;

    for (int i = 0; i < modelSalaire->rowCount(); ++i) {
        QString tranche = modelSalaire->data(modelSalaire->index(i, 0)).toString().trimmed();
        int count = modelSalaire->data(modelSalaire->index(i, 1)).toInt();

        if (tranche.isEmpty() || tranche == "< 1000")
            tranche = "<1000";

        compteParTranche[tranche] += count;
    }

    QPieSeries *seriesSalaire = new QPieSeries();
    seriesSalaire->setPieSize(0.7);
    seriesSalaire->setHoleSize(0.45);

    // Ajouter les tranches dans un ordre fixe pour la légende
    QStringList ordreTranches = {"<1000", "1000 - 1999", "2000 - 2999", ">= 3000"};
    QStringList tranchesLabels;
    for (const QString &trancheLabel : ordreTranches) {
        int count = compteParTranche.value(trancheLabel, 0);

        // Pour la légende QtCharts, "<" peut être interprété comme HTML.
        // On utilise donc "&lt;1000" pour l'affichage, ce qui donnera visuellement "<1000".
        QString legendLabel = trancheLabel;
        if (legendLabel == "<1000")
            legendLabel = "&lt;1000";

        tranchesLabels << legendLabel;
        seriesSalaire->append(legendLabel, count);
    }

    qreal totalSalaire = seriesSalaire->sum();
    seriesSalaire->setLabelsVisible(true);

    QVector<QColor> paletteSalaire = {
        QColor("#5e35b1"),   // violet
        QColor("#00897b"),   // turquoise
        QColor("#fdd835"),   // jaune
        QColor("#c62828")    // rouge foncé
    };
    for (int i = 0; i < seriesSalaire->count(); ++i) {
        QPieSlice *slice = seriesSalaire->slices().at(i);
        slice->setBrush(paletteSalaire.at(i % paletteSalaire.size()));

        double pourcentage = (totalSalaire > 0.0) ? (slice->value() * 100.0 / totalSalaire) : 0.0;
        slice->setLabel(QString("%1%")
                        .arg(pourcentage, 0, 'f', 1));
        slice->setLabelColor(Qt::black);
        slice->setLabelPosition(QPieSlice::LabelOutside);
    }

    QChart *chartSalaire = new QChart();
    chartSalaire->addSeries(seriesSalaire);
    chartSalaire->setTitle(tr("Répartition des employés par tranches de salaire"));
    QFont titreSalaireFont = chartSalaire->titleFont();
    titreSalaireFont.setBold(true);
    chartSalaire->setTitleFont(titreSalaireFont);
    chartSalaire->legend()->setAlignment(Qt::AlignRight);
    chartSalaire->legend()->setFont(QFont("Segoe UI", 9));
    // Forcer la légende à n'afficher que les tranches de salaire
    const auto markersSalaire = chartSalaire->legend()->markers(seriesSalaire);
    for (int i = 0; i < markersSalaire.size() && i < tranchesLabels.size(); ++i) {
        if (markersSalaire.at(i))
            markersSalaire.at(i)->setLabel(tranchesLabels.at(i));
    }
    chartSalaire->setBackgroundBrush(Qt::white);
    chartSalaire->setBackgroundPen(Qt::NoPen);
    chartSalaire->setMargins(QMargins(20, 40, 20, 40));

    QChartView *chartViewSalaire = new QChartView(chartSalaire, ui->widget_6);
    chartViewSalaire->setRenderHint(QPainter::Antialiasing);
    chartViewSalaire->setStyleSheet("background-color: white; border: 2px solid #5e35b1; border-radius: 8px;");
    chartViewSalaire->setMinimumHeight(360);
    chartViewSalaire->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartsRow->addWidget(chartViewSalaire, 1);
}

/*---------------------------------------------------
 * 📸 Face ID - Capturer une photo de visage
 *---------------------------------------------------*/
void Gemploye::captureFaceImage()
{
    // Créer un dialogue personnalisé pour la capture de photo
    QDialog *cameraDialog = new QDialog(this);
    cameraDialog->setWindowTitle(tr("Capture Face ID"));
    cameraDialog->setModal(true);
    cameraDialog->resize(640, 480);

    QVBoxLayout *layout = new QVBoxLayout(cameraDialog);

    // Widget vidéo pour afficher la caméra
    QVideoWidget *videoWidget = new QVideoWidget(cameraDialog);
    layout->addWidget(videoWidget);

    // Label pour les instructions
    QLabel *instructionLabel = new QLabel(tr("Positionnez votre visage devant la caméra"), cameraDialog);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2196F3;");
    layout->addWidget(instructionLabel);

    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *captureBtn = new QPushButton(tr("📸 Capturer"), cameraDialog);
    QPushButton *cancelBtn = new QPushButton(tr("Annuler"), cameraDialog);
    captureBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px 16px; font-weight: bold;");
    cancelBtn->setStyleSheet("background-color: #f44336; color: white; padding: 8px 16px; font-weight: bold;");
    buttonLayout->addWidget(captureBtn);
    buttonLayout->addWidget(cancelBtn);
    layout->addLayout(buttonLayout);

    // Configuration de la caméra
    QCamera *camera = new QCamera(cameraDialog);
    QMediaCaptureSession *captureSession = new QMediaCaptureSession(cameraDialog);
    QImageCapture *imageCapture = new QImageCapture(cameraDialog);

    captureSession->setCamera(camera);
    captureSession->setImageCapture(imageCapture);
    captureSession->setVideoOutput(videoWidget);

    // Démarrer la caméra
    camera->start();

    // Connexion du bouton Capturer
    connect(captureBtn, &QPushButton::clicked, [=]() {
        imageCapture->capture();
    });

    // Connexion du bouton Annuler
    connect(cancelBtn, &QPushButton::clicked, cameraDialog, &QDialog::reject);

    // Traitement de l'image capturée
    connect(imageCapture, &QImageCapture::imageCaptured, [=](int id, const QImage &preview) {
        Q_UNUSED(id);
        
        // Arrêter la caméra
        camera->stop();
        
        // Stocker l'image capturée
        m_capturedFaceImage = preview;
        
        QMessageBox::information(cameraDialog, tr("Capture réussie"), 
            tr("Photo du visage capturée avec succès!\nElle sera enregistrée lors de la sauvegarde de l'employé."));
        cameraDialog->accept();
    });

    // Gestion des erreurs de capture
    connect(imageCapture, &QImageCapture::errorOccurred, [=](int id, QImageCapture::Error error, const QString &errorString) {
        Q_UNUSED(id);
        Q_UNUSED(error);
        QMessageBox::critical(cameraDialog, tr("Erreur"), 
            tr("Erreur de capture: %1").arg(errorString));
        camera->stop();
        cameraDialog->reject();
    });

    cameraDialog->exec();
    delete cameraDialog;
}

/*---------------------------------------------------
 * 📸 Slot pour le bouton Capturer Face ID
 *---------------------------------------------------*/
void Gemploye::on_btnCaptureFace_clicked()
{
    captureFaceImage();
}

/*---------------------------------------------------
 * 🚪 Déconnexion - Retour à l'écran de login
 *---------------------------------------------------*/
void Gemploye::on_btnDeconnexion_clicked()
{
    // Demander confirmation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Déconnexion", 
                                   "Voulez-vous vraiment vous déconnecter ?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Fermer la fenêtre actuelle
        this->close();
        
        // Afficher le LoginDialog
        LoginDialog *loginDialog = new LoginDialog();
        loginDialog->show();
    }
}

/*---------------------------------------------------
 * 🚪 Déconnexion 2 - Retour à l'écran de login
 *---------------------------------------------------*/
void Gemploye::on_btnDeconnexion_2_clicked()
{
    // Demander confirmation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Déconnexion", 
                                   "Voulez-vous vraiment vous déconnecter ?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Fermer la fenêtre actuelle
        this->close();
        
        // Afficher le LoginDialog
        LoginDialog *loginDialog = new LoginDialog();
        loginDialog->show();
    }
}
