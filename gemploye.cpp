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
#include <QLabel>

Gemploye::Gemploye(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gemploye)
{
    ui->setupUi(this);

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

    ui->linecin->setValidator(new QRegularExpressionValidator(cinRegex, this));
    ui->linenom->setValidator(new QRegularExpressionValidator(nameRegex, this));
    ui->lineprenom->setValidator(new QRegularExpressionValidator(nameRegex, this));
    ui->linentel_2->setValidator(new QRegularExpressionValidator(nameRegex, this)); // poste
    ui->lineemail->setPlaceholderText("exemple@domaine.com");
    ui->linentel->setValidator(new QRegularExpressionValidator(telRegex, this));
    ui->linemdp->setPlaceholderText("8+ caractères, maj, min, chiffre, symbole");
    ui->linemdp->setEchoMode(QLineEdit::Password);

    // Validateur pour le salaire : nombre décimal positif avec 2 décimales max
    auto *salaryValidator = new QDoubleValidator(0, 1e9, 2, this);
    salaryValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->linentel_3->setValidator(salaryValidator);
}

Gemploye::~Gemploye()
{
    delete ui;
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

    // Vérifier si un champ est vide
    if (cin.isEmpty() || nom.isEmpty() || prenom.isEmpty() || email.isEmpty() ||
        tel.isEmpty() || mdp.isEmpty() || poste.isEmpty() || salaireStr.isEmpty()) {
        erreur = "Tous les champs doivent être remplis.";
        return false;
    }

    // Expressions régulières
    QRegularExpression cinRegex("^[0-9]{6,12}$");
    QRegularExpression nameRegex("^[A-Za-zÀ-ÖØ-öø-ÿ\\s\\-']{2,50}$");
    QRegularExpression emailRegex("^[\\w\\.\\-]+@[\\w\\-]+\\.[A-Za-z]{2,6}$");
    QRegularExpression telRegex("^\\+?[0-9]{7,15}$");
    QRegularExpression mdpRegex("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)(?=.*[^A-Za-z0-9]).{8,}$");

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
 * 🧩 Navigation entre pages du stackedWidget
 *---------------------------------------------------*/
void Gemploye::on_Employ_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void Gemploye::on_Employ_4_clicked() { ui->stackedWidget->setCurrentIndex(1); }
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

    employee e;

    // 🚫 Vérifie si le CIN existe déjà
    if (e.existe(cin)) {
        QMessageBox::warning(this, "Erreur", "Un employé avec ce CIN existe déjà !");
        return;
    }

    // ✅ Si non, on l'ajoute
    if (e.ajouter(cin, nom, prenom, email, n_tel, mdp, poste, salaire)) {
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
    if (e_global.modifier(selectedId, cin, nom, prenom, email, n_tel, mdp, poste, salaire)) {
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
    ui->linentel_2->setText(model->data(model->index(row, 7)).toString());
    ui->linentel_3->setText(model->data(model->index(row, 8)).toString());
}

/*---------------------------------------------------
 * ❌ Supprimer un employé
 *---------------------------------------------------*/
void Gemploye::on_supprimer_clicked()
{
    int cin = ui->linecin->text().toInt();

    if (cin == 0) {
        QMessageBox::warning(this, "Attention", "Veuillez entrer un numéro de CIN valide !");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation",
        "Voulez-vous vraiment supprimer cet employé ?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
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
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "employes.pdf", "Fichiers PDF (*.pdf)");
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

    QAbstractItemModel *model = ui->tableemp->model();
    if (!model) {
        QMessageBox::warning(this, "Attention", "Aucune donnée à exporter.");
        return;
    }

    int rows = model->rowCount();
    int cols = model->columnCount();

    // on saute la colonne 0 (ID_EMPLOYE) dans le PDF
    int firstCol = 1;

    int margin = 100;
    int x = margin;
    int y = margin;
    int rowHeight = 300;
    int printableWidth = pdfWriter.width() - 2 * margin;
    int visibleCols = cols - firstCol;
    if (visibleCols <= 0)
        visibleCols = cols; // sécurité

    int colWidth = printableWidth / visibleCols;

    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    for (int c = firstCol; c < cols; ++c) {
        QString header = model->headerData(c, Qt::Horizontal).toString();
        painter.drawText(x + (c - firstCol) * colWidth, y, colWidth, rowHeight,
                         Qt::AlignLeft | Qt::AlignVCenter, header);
    }

    y += rowHeight;

    for (int r = 0; r < rows; ++r) {
        for (int c = firstCol; c < cols; ++c) {
            QString data = model->data(model->index(r, c)).toString();
            painter.drawText(x + (c - firstCol) * colWidth, y, colWidth, rowHeight,
                             Qt::AlignLeft | Qt::AlignVCenter, data);
        }
        y += rowHeight;

        if (y > pdfWriter.height() - 200) {
            pdfWriter.newPage();
            y = 100;
        }
    }

    painter.end();

    QMessageBox::information(this, "Succès", "PDF exporté avec succès.");
}

void Gemploye::afficherStatistiques()
{
    // Nettoyer le contenu précédent
    QLayout *oldLayout = ui->widget_6->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    auto *layout = new QVBoxLayout(ui->widget_6);
    ui->widget_6->setLayout(layout);

    // Style commun pour les tableaux de stats (reprend celui de tableemp)
    QString tableStyle = ui->tableemp->styleSheet();

    // 1) Stat par poste : nombre d'employés et salaire moyen par poste
    QSqlQueryModel *modelPoste = new QSqlQueryModel(ui->widget_6);
    modelPoste->setQuery(
        "SELECT POSTE, COUNT(*) AS \"Nb employés\", "
        "       ROUND(AVG(SALAIRE), 2) AS \"Salaire moyen\" "
        "FROM EMPLOYE "
        "GROUP BY POSTE "
        "ORDER BY COUNT(*) DESC");

    modelPoste->setHeaderData(0, Qt::Horizontal, QObject::tr("Poste"));
    modelPoste->setHeaderData(1, Qt::Horizontal, QObject::tr("Nb employés"));
    modelPoste->setHeaderData(2, Qt::Horizontal, QObject::tr("Salaire moyen"));

    QTableView *viewPoste = new QTableView(ui->widget_6);
    viewPoste->setModel(modelPoste);
    viewPoste->setStyleSheet(tableStyle);
    viewPoste->setSelectionBehavior(QAbstractItemView::SelectRows);
    viewPoste->setSelectionMode(QAbstractItemView::SingleSelection);
    viewPoste->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewPoste->horizontalHeader()->setStretchLastSection(true);
    viewPoste->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(viewPoste, 1);

    // 2) Stat par tranches de salaire (calculées en SQL)
    QSqlQueryModel *modelSalaire = new QSqlQueryModel(ui->widget_6);
    modelSalaire->setQuery(
        "SELECT "
        "  CASE "
        "    WHEN SALAIRE < 1000 THEN '< 1000' "
        "    WHEN SALAIRE < 2000 THEN '1000 - 1999' "
        "    ELSE '>= 2000' "
        "  END AS TRANCHE, "
        "  COUNT(*) AS \"Nb employés\" "
        "FROM EMPLOYE "
        "GROUP BY CASE "
        "    WHEN SALAIRE < 1000 THEN '< 1000' "
        "    WHEN SALAIRE < 2000 THEN '1000 - 1999' "
        "    ELSE '>= 2000' "
        "  END "
        "ORDER BY TRANCHE");

    modelSalaire->setHeaderData(0, Qt::Horizontal, QObject::tr("Tranche salaire"));
    modelSalaire->setHeaderData(1, Qt::Horizontal, QObject::tr("Nb employés"));

    QTableView *viewSalaire = new QTableView(ui->widget_6);
    viewSalaire->setModel(modelSalaire);
    viewSalaire->setStyleSheet(tableStyle);
    viewSalaire->setSelectionBehavior(QAbstractItemView::SelectRows);
    viewSalaire->setSelectionMode(QAbstractItemView::SingleSelection);
    viewSalaire->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewSalaire->horizontalHeader()->setStretchLastSection(true);
    viewSalaire->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(viewSalaire, 1);
}
