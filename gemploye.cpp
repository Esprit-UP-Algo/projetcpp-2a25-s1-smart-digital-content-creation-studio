#include "gemploye.h"
#include "ui_gemploye.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QPixmap>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QPieSlice>
#include <QPainter>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QDir>
#include <QDateTime>
#include <QFont>
#include <QFontMetrics>
#include <QPdfWriter>
#include <QPageSize>
#include <QPageLayout>
#include <QBrush>
#include <QApplication>
#include <QWidget>

// Constructeur
Gemploye::Gemploye(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gemploye),
    selectedReference(""),
    selectedCin(""),
    selectedCreateurId(-1),
    createurChartView(nullptr)
{
    ui->setupUi(this);

    // Logo (adapter le chemin si besoin)
    QPixmap logo("C:/Users/LOQ/Documents/Gemploye/images/logo.png");
    if (!logo.isNull())
        ui->label_7->setPixmap(logo.scaled(ui->label_7->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // VALIDATEURS EMPLOYE (stricts)
    {
        QRegularExpression reCin("^[0-9]{8}$");
        ui->lineEdit->setValidator(new QRegularExpressionValidator(reCin, this)); // CIN
        ui->lineEdit->setMaxLength(8);
    }
    {
        QRegularExpression reNom("^[A-Za-zÀ-ÿ' -]{1,50}$");
        ui->lineEdit_2->setValidator(new QRegularExpressionValidator(reNom, this)); // nom
        ui->lineEdit_2->setMaxLength(50);
        ui->lineEdit_3->setValidator(new QRegularExpressionValidator(reNom, this)); // prenom
        ui->lineEdit_3->setMaxLength(50);
    }
    {
        QRegularExpression reMail(R"(^(?:[A-Za-z0-9._%+\-]+)@(outlook|yahoo|gmail)\.(fr|com)$)", QRegularExpression::CaseInsensitiveOption);
        ui->lineEdit_4->setValidator(new QRegularExpressionValidator(reMail, this)); // email
    }
    {
        QRegularExpression reTel("^[0-9]{8}$");
        ui->lineEdit_6->setValidator(new QRegularExpressionValidator(reTel, this)); // N_gsm
        ui->lineEdit_6->setMaxLength(8);
    }

    // TABLE EMPLOYES -> tableWidget
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels({"CIN","Nom","Prenom","Email","N Tel","Mot de passe"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    Employe::afficher(ui->tableWidget);

    // VALIDATEURS MATERIEL
    QRegularExpression regexChiffres("^[0-9]+$");
    ui->lineEdit_11->setValidator(new QRegularExpressionValidator(regexChiffres, this)); // REFERENCE

    QRegularExpression regexLettres("^[a-zA-ZÀ-ÿ ]{0,20}$");
    ui->lineEdit_9->setValidator(new QRegularExpressionValidator(regexLettres, this)); // TYPE_APPAREIL
    ui->lineEdit_9->setMaxLength(20);
    ui->lineEdit_10->setValidator(new QRegularExpressionValidator(regexLettres, this)); // MARQUE
    ui->lineEdit_10->setMaxLength(20);

    // TABLE MATERIEL -> tableWidget_6
    ui->tableWidget_6->setColumnCount(4);
    ui->tableWidget_6->setHorizontalHeaderLabels({"Type Appareil","Reference","Marque","Disponibilite"});
    ui->tableWidget_6->horizontalHeader()->setStretchLastSection(true);
    Mtmp.afficher(ui->tableWidget_6);

    // VALIDATEURS CREATEUR
    {
        QRegularExpression reCode("^\\d{1,10}$");
        ui->lineEdit_12->setValidator(new QRegularExpressionValidator(reCode, this));
        ui->lineEdit_12->setMaxLength(10);
    }
    {
        QRegularExpression reNom("^[A-Za-zÀ-ÿ' -]{1,50}$");
        ui->lineEdit_28->setValidator(new QRegularExpressionValidator(reNom, this));
        ui->lineEdit_28->setMaxLength(50);
        ui->lineEdit_27->setValidator(new QRegularExpressionValidator(reNom, this));
        ui->lineEdit_27->setMaxLength(50);
    }
    {
        QRegularExpression reCategorie("^[A-Za-zÀ-ÿ0-9' -]{0,50}$");
        ui->lineEdit_29->setValidator(new QRegularExpressionValidator(reCategorie, this));
        ui->lineEdit_29->setMaxLength(50);
    }
    {
        QRegularExpression reAbonnes("^\\d{1,10}$");
        ui->lineEdit_30->setValidator(new QRegularExpressionValidator(reAbonnes, this));
        ui->lineEdit_30->setMaxLength(10);
    }

    ui->dateEdit_3->setDate(QDate::currentDate());

    // TABLE CREATEURS -> tableWidget_5
    ui->tableWidget_5->setColumnCount(6);
    ui->tableWidget_5->setHorizontalHeaderLabels({"Code unique","Nom","Prenom","Date d'adhesion","Categorie","Abonnes"});
    ui->tableWidget_5->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_5->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_5->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_5->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->comboBox_6->clear();
    ui->comboBox_6->addItem("Code unique", "CODE_UNIQUE");
    ui->comboBox_6->addItem("Nom", "NOM");
    ui->comboBox_6->addItem("Date d'adhésion", "DATE_ADHESION");
    ui->comboBox_6->addItem("Abonnés", "ABONNEES");
    ui->comboBox_6->setCurrentIndex(0);
    ui->comboBox_6->setMinimumWidth(150);

    // Ensure dropdown view is visible and properly sized
    if (ui->comboBox_6->view()) {
        ui->comboBox_6->view()->setMinimumWidth(200);
        ui->comboBox_6->view()->setStyleSheet(
            "QAbstractItemView {"
            " background-color: white;"
            " color: black;"
            " border: 1px solid #c4c4c4;"
            " border-radius: 8px;"
            " selection-background-color: #ff8c00;"
            " selection-color: white;"
            " padding: 5px;"
            "}"
            "QAbstractItemView::item {"
            " padding: 8px;"
            " border-radius: 4px;"
            " min-height: 25px;"
            "}"
            "QAbstractItemView::item:hover {"
            " background-color: #ffa500;"
            " color: white;"
            "}"
            "QAbstractItemView::item:selected {"
            " background-color: #ff8c00;"
            " color: white;"
            "}"
        );
    }

    refreshCreateurTable();
    ui->pushButton_19->setText("Ajouter");
    ui->pushButton_25->setText("Supprimer");

    // Initialize createur statistics chart
    if (ui->widget_4) {
        QVBoxLayout *chartLayout = nullptr;
        if (ui->widget_4->layout()) {
            chartLayout = qobject_cast<QVBoxLayout*>(ui->widget_4->layout());
        }
        if (!chartLayout) {
            chartLayout = new QVBoxLayout(ui->widget_4);
            chartLayout->setContentsMargins(0, 0, 0, 0);
        }
        createurChartView = new QChartView(ui->widget_4);
        createurChartView->setRenderHint(QPainter::Antialiasing);
        chartLayout->addWidget(createurChartView);
        updateCreateurStats();
    }

    // Connect tab change to update stats
    if (ui->Widget) {
        connect(ui->Widget, &QTabWidget::currentChanged, this, [this](int index) {
            if (index == 1) { // Statistics tab (tab_10 is index 1)
                updateCreateurStats();
            }
        });
    }

    // Connexions explicites
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &Gemploye::on_tableWidget_cellClicked);
    connect(ui->pushButton, &QPushButton::clicked, this, &Gemploye::on_pushButton_clicked);
    connect(ui->pushButton_20, &QPushButton::clicked, this, &Gemploye::on_pushButton_20_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Gemploye::on_pushButton_2_clicked);
    connect(ui->tableWidget_6, &QTableWidget::cellClicked, this, &Gemploye::on_tableWidget_6_cellClicked);
}

Gemploye::~Gemploye()
{
    delete ui;
}

// Navigation
void Gemploye::on_Employ_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Gemploye::on_Employ_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Gemploye::on_Employ_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Gemploye::on_Employ_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Gemploye::on_Employ_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void Gemploye::on_Employ_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

// Employe : Ajouter
void Gemploye::on_pushButton_clicked()
{
    const QString cin = ui->lineEdit->text().trimmed();
    const QString nom = ui->lineEdit_2->text().trimmed();
    const QString prenom = ui->lineEdit_3->text().trimmed();
    const QString email = ui->lineEdit_4->text().trimmed();
    const QString pass = ui->lineEdit_5->text();
    const QString gsm = ui->lineEdit_6->text().trimmed();

    // Obligatoires
    if (cin.isEmpty() || nom.isEmpty() || prenom.isEmpty() || email.isEmpty() || pass.isEmpty() || gsm.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants", "Tous les champs sont obligatoires.");
        return;
    }

    // Controles stricts
    if (!Employe::cinValide(cin)) {
        QMessageBox::warning(this, "CIN invalide", "Le CIN doit contenir exactement 8 chiffres.");
        return;
    }

    if (!Employe::nomPrenomValide(nom) || !Employe::nomPrenomValide(prenom)) {
        QMessageBox::warning(this, "Nom/Prenom invalides", "Utilise lettres (accents possibles), espaces, apostrophes ou tirets.");
        return;
    }

    if (!Employe::emailValide(email)) {
        QMessageBox::warning(this, "Email invalide", "Autorises: outlook, yahoo ou gmail; TLD .fr ou .com.");
        return;
    }

    if (!Employe::telValide(gsm)) {
        QMessageBox::warning(this, "Telephone invalide", "Le numero doit contenir exactement 8 chiffres.");
        return;
    }

    if (Employe::existeCin(cin)) {
        QMessageBox::warning(this, "CIN en double", "Un employe avec ce CIN existe deja.");
        return;
    }

    Employe e(cin, nom, prenom, email, pass, gsm);
    if (e.ajouter()) {
        QMessageBox::information(this, "Succes", "Employe ajoute.");
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
        Employe::afficher(ui->tableWidget);
    } else {
        QMessageBox::critical(this, "Erreur", "Insertion refusee par la base.");
    }
}

// Employe : clic ligne -> remplit QLineEdit
void Gemploye::on_tableWidget_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    auto *tbl = ui->tableWidget;
    if (row < 0 || row >= tbl->rowCount()) return;

    const QString cin = tbl->item(row, 0) ? tbl->item(row, 0)->text() : QString();
    const QString nom = tbl->item(row, 1) ? tbl->item(row, 1)->text() : QString();
    const QString prenom = tbl->item(row, 2) ? tbl->item(row, 2)->text() : QString();
    const QString email = tbl->item(row, 3) ? tbl->item(row, 3)->text() : QString();
    const QString gsm = tbl->item(row, 4) ? tbl->item(row, 4)->text() : QString();
    const QString pass = tbl->item(row, 5) ? tbl->item(row, 5)->text() : QString();

    ui->lineEdit->setText(cin);
    ui->lineEdit_2->setText(nom);
    ui->lineEdit_3->setText(prenom);
    ui->lineEdit_4->setText(email);
    ui->lineEdit_6->setText(gsm);
    ui->lineEdit_5->setText(pass);

    selectedCin = cin;
}

// Employe : Modifier
void Gemploye::on_pushButton_20_clicked()
{
    if (selectedCin.isEmpty()) {
        QMessageBox::warning(this, "Selection", "Selectionne un employe dans le tableau.");
        return;
    }

    const QString newCin = ui->lineEdit->text().trimmed();
    const QString nom = ui->lineEdit_2->text().trimmed();
    const QString prenom = ui->lineEdit_3->text().trimmed();
    const QString email = ui->lineEdit_4->text().trimmed();
    const QString pass = ui->lineEdit_5->text();
    const QString gsm = ui->lineEdit_6->text().trimmed();

    if (newCin.isEmpty() || nom.isEmpty() || prenom.isEmpty() || email.isEmpty() || pass.isEmpty() || gsm.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants", "Tous les champs sont obligatoires.");
        return;
    }

    if (!Employe::cinValide(newCin)) {
        QMessageBox::warning(this, "CIN invalide", "Le CIN doit contenir exactement 8 chiffres.");
        return;
    }

    if (!Employe::nomPrenomValide(nom) || !Employe::nomPrenomValide(prenom)) {
        QMessageBox::warning(this, "Nom/Prenom invalides", "Utilise lettres (accents possibles), espaces, apostrophes ou tirets.");
        return;
    }

    if (!Employe::emailValide(email)) {
        QMessageBox::warning(this, "Email invalide", "Autorises: outlook, yahoo ou gmail; TLD .fr ou .com.");
        return;
    }

    if (!Employe::telValide(gsm)) {
        QMessageBox::warning(this, "Telephone invalide", "Le numero doit contenir exactement 8 chiffres.");
        return;
    }

    if (newCin != selectedCin && Employe::existeCin(newCin)) {
        QMessageBox::warning(this, "Conflit CIN", "Un employe avec ce CIN existe deja.");
        return;
    }

    Employe e(newCin, nom, prenom, email, pass, gsm);
    if (e.modifier(selectedCin)) {
        QMessageBox::information(this, "Succes", "Employe modifie.");
        selectedCin = newCin;
        Employe::afficher(ui->tableWidget);
    } else {
        QMessageBox::critical(this, "Erreur", "Echec de la modification.");
    }
}

// Employe : Supprimer
void Gemploye::on_pushButton_2_clicked()
{
    if (selectedCin.isEmpty()) {
        QMessageBox::warning(this, "Selection", "Selectionne un employe dans le tableau.");
        return;
    }

    const int ret = QMessageBox::question(this, "Supprimer", "Voulez-vous vraiment supprimer cet employe ?", QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    if (Employe::supprimer(selectedCin)) {
        QMessageBox::information(this, "Succes", "Employe supprime.");
        selectedCin.clear();
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
        Employe::afficher(ui->tableWidget);
    } else {
        QMessageBox::critical(this, "Erreur", "Echec de la suppression. Verifie les contraintes de foreign key eventuelles.");
    }
}

// Materiel : Ajouter
void Gemploye::on_pushButton_8_clicked()
{
    if (ui->lineEdit_9->text().isEmpty() || ui->lineEdit_10->text().isEmpty() || ui->lineEdit_11->text().isEmpty()) {
        QMessageBox::warning(this, "Champ vide", "Veuillez remplir tous les champs !");
        return;
    }

    const QString type = ui->lineEdit_9->text();
    const QString ref = ui->lineEdit_11->text();
    const QString marque = ui->lineEdit_10->text();
    const QString dispo = ui->comboBox_10->currentText();

    if (type.length() > 20 || marque.length() > 20) {
        QMessageBox::warning(this, "Erreur", "Type ou marque trop long, max 20 caracteres.");
        return;
    }

    Materiel M(type, ref, marque, dispo);
    if (M.ajouter()) {
        QMessageBox::information(this, "Succes", "Materiel ajoute !");
        ui->lineEdit_9->clear();
        ui->lineEdit_10->clear();
        ui->lineEdit_11->clear();
        ui->comboBox_10->setCurrentIndex(0);
        Mtmp.afficher(ui->tableWidget_6);
    } else {
        QMessageBox::critical(this, "Erreur", "Echec de l'ajout !");
    }
}

// Materiel : clic ligne -> remplit champs
void Gemploye::on_tableWidget_6_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    ui->lineEdit_9->setText(ui->tableWidget_6->item(row, 0)->text()); // TYPE_APPAREIL
    ui->lineEdit_11->setText(ui->tableWidget_6->item(row, 1)->text()); // REFERENCE
    ui->lineEdit_10->setText(ui->tableWidget_6->item(row, 2)->text()); // MARQUE
    selectedReference = ui->tableWidget_6->item(row, 1)->text();

    const int idx = ui->comboBox_10->findText(ui->tableWidget_6->item(row, 3)->text());
    if (idx != -1)
        ui->comboBox_10->setCurrentIndex(idx);
}

// Materiel : Modifier
void Gemploye::on_pushButton_11_clicked()
{
    if (selectedReference.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez selectionner un materiel dans le tableau !");
        return;
    }

    const QString type = ui->lineEdit_9->text();
    const QString ref = ui->lineEdit_11->text();
    const QString marque = ui->lineEdit_10->text();
    const QString dispo = ui->comboBox_10->currentText();

    if (type.isEmpty() || marque.isEmpty() || ref.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs !");
        return;
    }

    Materiel M(type, ref, marque, dispo);
    if (M.modifier(selectedReference)) {
        QMessageBox::information(this, "Succes", "Materiel modifie !");
        selectedReference = ref;
        Mtmp.afficher(ui->tableWidget_6);
    } else {
        QMessageBox::critical(this, "Erreur", "Echec de la modification !");
    }
}

// Materiel : Supprimer
void Gemploye::on_pushButton_9_clicked()
{
    if (selectedReference.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez selectionner un materiel a supprimer !");
        return;
    }

    const int ret = QMessageBox::question(this, "Supprimer", "Voulez-vous vraiment supprimer ce materiel ?", QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        Materiel M;
        if (M.supprimer(selectedReference)) {
            QMessageBox::information(this, "Succes", "Materiel supprime !");
            selectedReference.clear();
            Mtmp.afficher(ui->tableWidget_6);
            ui->lineEdit_9->clear();
            ui->lineEdit_10->clear();
            ui->lineEdit_11->clear();
            ui->comboBox_10->setCurrentIndex(0);
        } else {
            QMessageBox::critical(this, "Erreur", "Echec de la suppression !");
        }
    }
}

// Createur : helpers
void Gemploye::clearCreateurForm()
{
    selectedCreateurId = -1;
    ui->lineEdit_12->clear();
    ui->lineEdit_28->clear();
    ui->lineEdit_27->clear();
    ui->lineEdit_29->clear();
    ui->lineEdit_30->clear();
    ui->dateEdit_3->setDate(QDate::currentDate());
    ui->tableWidget_5->clearSelection();
    ui->pushButton_19->setText("Ajouter");
}

void Gemploye::refreshCreateurTable()
{
    if (!ui->tableWidget_5) return;

    const QString search = ui->lineEdit_31 ? ui->lineEdit_31->text() : QString();
    QString orderField;
    if (ui->comboBox_6 && ui->comboBox_6->currentIndex() >= 0)
        orderField = ui->comboBox_6->currentData().toString();

    Createur::afficher(ui->tableWidget_5, search, orderField);
    updateCreateurStats();
}

// Createur : clic ligne -> remplit QLineEdit
void Gemploye::on_tableWidget_5_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    auto *tbl = ui->tableWidget_5;
    if (!tbl || row < 0 || row >= tbl->rowCount()) return;

    auto *codeItem = tbl->item(row, 0);
    if (!codeItem) return;

    selectedCreateurId = codeItem->data(Qt::UserRole).toInt();
    ui->lineEdit_12->setText(codeItem->text());
    ui->lineEdit_28->setText(tbl->item(row, 1) ? tbl->item(row, 1)->text() : QString());
    ui->lineEdit_27->setText(tbl->item(row, 2) ? tbl->item(row, 2)->text() : QString());
    ui->lineEdit_29->setText(tbl->item(row, 4) ? tbl->item(row, 4)->text() : QString());
    ui->lineEdit_30->setText(tbl->item(row, 5) ? tbl->item(row, 5)->text() : QString());

    const QString dateStr = tbl->item(row, 3) ? tbl->item(row, 3)->text() : QString();
    const QDate date = QDate::fromString(dateStr, "dd/MM/yyyy");
    if (date.isValid())
        ui->dateEdit_3->setDate(date);

    ui->pushButton_19->setText("Mettre a jour");
}

// Createur : Ajouter ou Modifier
void Gemploye::on_pushButton_19_clicked()
{
    const QString codeText = ui->lineEdit_12->text().trimmed();
    const QString nom = ui->lineEdit_28->text().trimmed();
    const QString prenom = ui->lineEdit_27->text().trimmed();
    const QString categorie = ui->lineEdit_29->text().trimmed();
    const QString abonnesText = ui->lineEdit_30->text().trimmed();
    const QDate dateAdhesion = ui->dateEdit_3->date();

    if (codeText.isEmpty() || nom.isEmpty() || prenom.isEmpty() || abonnesText.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants", "Code, nom, prenom et abonnes sont obligatoires.");
        return;
    }

    if (!Createur::codeUniqueValide(codeText)) {
        QMessageBox::warning(this, "Code invalide", "Le code unique doit contenir uniquement des chiffres (max 10).");
        return;
    }

    if (!Createur::nomPrenomValide(nom) || !Createur::nomPrenomValide(prenom)) {
        QMessageBox::warning(this, "Nom/Prenom invalides", "Utilise lettres (accents possibles), espaces, apostrophes ou tirets.");
        return;
    }

    if (!Createur::categorieValide(categorie)) {
        QMessageBox::warning(this, "Categorie invalide", "La categorie ne doit contenir que lettres/chiffres simples.");
        return;
    }

    if (!Createur::abonnesValide(abonnesText)) {
        QMessageBox::warning(this, "Abonnes invalides", "Le nombre d'abonnes doit etre un entier positif (max 10 chiffres).");
        return;
    }

    const int codeUnique = codeText.toInt();
    const int abonnes = abonnesText.toInt();
    const bool isEdit = (selectedCreateurId >= 0);

    if (!isEdit && Createur::codeUniqueExiste(codeUnique)) {
        QMessageBox::warning(this, "Code en double", "Un createur avec ce code unique existe deja.");
        return;
    }

    if (isEdit && Createur::codeUniqueExiste(codeUnique, selectedCreateurId)) {
        QMessageBox::warning(this, "Conflit code", "Ce code unique est deja attribue a un autre createur.");
        return;
    }

    Createur c(codeUnique, nom, prenom, dateAdhesion, categorie, abonnes);
    if (isEdit)
        c.setId(selectedCreateurId);

    const bool ok = isEdit ? c.modifier() : c.ajouter();
    if (ok) {
        QMessageBox::information(this, "Succes", isEdit ? "Createur modifie." : "Createur ajoute.");
        clearCreateurForm();
        refreshCreateurTable();
    } else {
        QMessageBox::critical(this, "Erreur", isEdit ? "La modification a echoue." : "L'ajout a echoue.");
    }
}

// Createur : Supprimer
void Gemploye::on_pushButton_25_clicked()
{
    if (selectedCreateurId < 0) {
        QMessageBox::warning(this, "Selection", "Selectionne un createur dans le tableau.");
        return;
    }

    const int ret = QMessageBox::question(this, "Supprimer", "Voulez-vous vraiment supprimer ce createur ?", QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    if (Createur::supprimer(selectedCreateurId)) {
        QMessageBox::information(this, "Succes", "Createur supprime.");
        clearCreateurForm();
        refreshCreateurTable();
    } else {
        QMessageBox::critical(this, "Erreur", "Echec de la suppression.");
    }
}

void Gemploye::on_lineEdit_31_textChanged(const QString& text)
{
    Q_UNUSED(text);
    refreshCreateurTable();
}

void Gemploye::on_comboBox_6_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    refreshCreateurTable();
}

void Gemploye::updateCreateurStats()
{
    if (!createurChartView) return;

    QSqlQuery query;
    query.prepare(R"(SELECT COALESCE(CATEGORIE, 'Non classe') AS categorie, SUM(ABONNEES) AS total_abonnes FROM CREATEUR GROUP BY COALESCE(CATEGORIE, 'Non classe') ORDER BY total_abonnes DESC)");
    if (!query.exec()) {
        qWarning() << "updateCreateurStats error:" << query.lastError().text();
        return;
    }

    auto *series = new QPieSeries();
    int sliceCount = 0;
    double others = 0.0;

    while (query.next()) {
        const QString categorie = query.value(0).toString();
        const double total = query.value(1).toDouble();
        if (total <= 0) continue;

        if (sliceCount < 5) {
            auto *slice = series->append(categorie, total);
            slice->setLabel(QString("%1 (%2)").arg(categorie).arg(static_cast<int>(total)));
            sliceCount++;
        } else {
            others += total;
        }
    }

    if (others > 0.0)
        series->append("Autres", others);

    if (series->slices().isEmpty())
        series->append("Aucun abonné", 1.0);

    series->setLabelsVisible(true);

    auto *chart = new QChart();
    chart->setTitle("Repartition des abonnes par categorie");
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    createurChartView->setChart(chart);
}

void Gemploye::on_pushButton_21_clicked()
{
    exportCreateurToPDF();
}

void Gemploye::exportCreateurToPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", 
                                                   QDir::homePath() + "/Rapport_Createurs.pdf", 
                                                   "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);
    pdfWriter.setResolution(300);
    
    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const int pageWidth = pdfWriter.width();
    const int pageHeight = pdfWriter.height();
    const int margin = 100;
    int yPos = margin;

    // Professional color palette
    QColor primaryOrange(255, 140, 0);
    QColor darkOrange(220, 120, 0);
    QColor lightGray(250, 250, 250);
    QColor borderGray(200, 200, 200);
    QColor textDark(30, 30, 30);
    QColor headerBg(245, 245, 245);

    // Professional typography with MUCH LARGER, readable fonts
    QFont titleFont("Arial", 28, QFont::Bold);
    QFont subtitleFont("Arial", 12, QFont::Normal);
    QFont sectionFont("Arial", 18, QFont::Bold);
    QFont tableHeaderFont("Arial", 11, QFont::Bold);
    QFont tableFont("Arial", 10, QFont::Normal);
    QFont statsLabelFont("Arial", 12, QFont::Bold);
    QFont statsValueFont("Arial", 14, QFont::Bold);

    // ========== HEADER SECTION ==========
    QRect headerRect(margin, yPos, pageWidth - 2*margin, 100);
    painter.fillRect(headerRect, primaryOrange);
    
    // Title
    painter.setFont(titleFont);
    painter.setPen(Qt::white);
    QRect titleRect = headerRect.adjusted(0, 25, 0, -50);
    painter.drawText(titleRect, Qt::AlignCenter | Qt::AlignVCenter, 
                     "RAPPORT DES CRÉATEURS DE CONTENU");
    
    // Subtitle with date
    painter.setFont(subtitleFont);
    QString dateStr = QString("Généré le %1")
                      .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy à HH:mm"));
    QRect dateRect = headerRect.adjusted(0, 70, 0, -15);
    painter.drawText(dateRect, Qt::AlignCenter, dateStr);
    
    yPos += 130; // MUCH MORE space after header

    // ========== TABLE SECTION ==========
    // Section header with background
    QRect sectionHeaderRect(margin, yPos, pageWidth - 2*margin, 50);
    painter.fillRect(sectionHeaderRect, headerBg);
    
    painter.setFont(sectionFont);
    painter.setPen(textDark);
    painter.drawText(sectionHeaderRect.adjusted(20, 0, -20, 0), 
                       Qt::AlignLeft | Qt::AlignVCenter, "LISTE DES CRÉATEURS");
    
    // Accent line
    painter.setPen(QPen(primaryOrange, 4));
    painter.drawLine(margin + 20, yPos + 42, margin + 280, yPos + 42);
    
    yPos += 70; // MUCH MORE space after section header

    // Get data
    QSqlQuery query;
    query.prepare("SELECT CODE_UNIQUE, NOM, PRENOM, DATE_ADHESION, CATEGORIE, ABONNEES FROM CREATEUR ORDER BY ID_CREATEUR");
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", 
                              "Impossible de récupérer les données: " + query.lastError().text());
        painter.end();
        return;
    }

    // Table configuration with better proportions
    QStringList headers = {"Code", "Nom", "Prénom", "Date d'adhésion", "Catégorie", "Abonnés"};
    QList<int> colWidths = {90, 140, 140, 140, 170, 100};
    const int colCount = headers.size();
    const int tableWidth = pageWidth - 2*margin;
    
    // Scale column widths
    int totalWidth = 0;
    for (int w : colWidths) totalWidth += w;
    QList<int> scaledWidths;
    for (int w : colWidths) {
        scaledWidths.append((w * (tableWidth - 20)) / totalWidth);
    }

    const int rowHeight = 45; // LARGER row height
    const int headerHeight = 48; // LARGER header
    const int cellPadding = 15; // MORE padding
    int tableStartY = yPos;

    // Table header with professional styling
    QRect tableHeaderRect(margin, yPos, tableWidth, headerHeight);
    painter.fillRect(tableHeaderRect, darkOrange);
    
    // Header borders
    painter.setPen(QPen(primaryOrange, 3));
    painter.drawLine(margin, yPos, margin + tableWidth, yPos);
    
    painter.setFont(tableHeaderFont);
    painter.setPen(Qt::white);
    
    int xPos = margin;
    for (int i = 0; i < colCount; ++i) {
        QRect headerCellRect(xPos + cellPadding, yPos + 12, scaledWidths[i] - cellPadding * 2, headerHeight - 24);
        painter.drawText(headerCellRect, Qt::AlignLeft | Qt::AlignVCenter, headers[i]);
        
        // Vertical separator
        if (i < colCount - 1) {
            painter.setPen(QPen(Qt::white, 1.5));
            painter.drawLine(xPos + scaledWidths[i], yPos + 6, xPos + scaledWidths[i], yPos + headerHeight - 6);
        }
        xPos += scaledWidths[i];
    }
    
    // Header bottom border
    painter.setPen(QPen(primaryOrange, 3));
    painter.drawLine(margin, yPos + headerHeight, margin + tableWidth, yPos + headerHeight);
    
    yPos += headerHeight + 10; // MORE space after header

    // Table rows
    painter.setFont(tableFont);
    painter.setPen(textDark);
    
    int rowNum = 0;
    int totalCreateurs = 0;
    int totalAbonnes = 0;
    int maxAbonnes = 0;
    
    while (query.next()) {
        totalCreateurs++;
        
        // Page break check
        if (yPos + rowHeight > pageHeight - margin - 200) {
            // Draw table border before page break
            painter.setPen(QPen(borderGray, 2));
            painter.drawRect(margin, tableStartY, tableWidth, yPos - tableStartY - 10);
            
            pdfWriter.newPage();
            yPos = margin;
            tableStartY = yPos;
            
            // Redraw section header
            QRect newSectionRect(margin, yPos, pageWidth - 2*margin, 50);
            painter.fillRect(newSectionRect, headerBg);
            painter.setFont(sectionFont);
            painter.setPen(textDark);
            painter.drawText(newSectionRect.adjusted(20, 0, -20, 0), 
                            Qt::AlignLeft | Qt::AlignVCenter, "LISTE DES CRÉATEURS");
            painter.setPen(QPen(primaryOrange, 4));
            painter.drawLine(margin + 20, yPos + 42, margin + 280, yPos + 42);
            yPos += 70;
            
            // Redraw table header
            QRect newHeaderRect(margin, yPos, tableWidth, headerHeight);
            painter.fillRect(newHeaderRect, darkOrange);
            painter.setPen(QPen(primaryOrange, 3));
            painter.drawLine(margin, yPos, margin + tableWidth, yPos);
            
            painter.setFont(tableHeaderFont);
            painter.setPen(Qt::white);
            xPos = margin;
            for (int i = 0; i < colCount; ++i) {
                QRect headerCellRect(xPos + cellPadding, yPos + 12, scaledWidths[i] - cellPadding * 2, headerHeight - 24);
                painter.drawText(headerCellRect, Qt::AlignLeft | Qt::AlignVCenter, headers[i]);
                if (i < colCount - 1) {
                    painter.setPen(QPen(Qt::white, 1.5));
                    painter.drawLine(xPos + scaledWidths[i], yPos + 6, xPos + scaledWidths[i], yPos + headerHeight - 6);
                }
                xPos += scaledWidths[i];
            }
            painter.setPen(QPen(primaryOrange, 3));
            painter.drawLine(margin, yPos + headerHeight, margin + tableWidth, yPos + headerHeight);
            yPos += headerHeight + 10;
            
            painter.setFont(tableFont);
            painter.setPen(textDark);
        }

        // Alternate row background
        QRect rowRect(margin, yPos, tableWidth, rowHeight);
        if (rowNum % 2 == 0) {
            painter.fillRect(rowRect, Qt::white);
        } else {
            painter.fillRect(rowRect, lightGray);
        }

        // Draw cell content
        xPos = margin;
        QFontMetrics metrics(tableFont);
        
        for (int col = 0; col < colCount; ++col) {
            QVariant value = query.value(col);
            QString cellText;
            
            if (value.isNull()) {
                cellText = "-";
            } else {
                // Format date properly
                if (col == 3) { // Date column
                    QDate date = value.toDate();
                    if (date.isValid()) {
                        cellText = date.toString("dd/MM/yyyy");
                    } else {
                        QString dateStr = value.toString();
                        if (dateStr.contains("T")) {
                            date = QDate::fromString(dateStr.left(10), "yyyy-MM-dd");
                            if (date.isValid()) {
                                cellText = date.toString("dd/MM/yyyy");
                            } else {
                                cellText = dateStr.left(10);
                            }
                        } else {
                            cellText = dateStr;
                        }
                    }
                } else {
                    cellText = value.toString();
                }
                
                // Handle empty category
                if (col == 4 && cellText.trimmed().isEmpty()) {
                    cellText = "-";
                }
                
                // Handle subscribers count
                if (col == 5) { // Abonnés column
                    bool ok;
                    int abonnes = cellText.toInt(&ok);
                    if (ok) {
                        totalAbonnes += abonnes;
                        if (abonnes > maxAbonnes) {
                            maxAbonnes = abonnes;
                        }
                    }
                }
            }
            
            QRect cellRect(xPos + cellPadding, yPos + 10, scaledWidths[col] - cellPadding * 2, rowHeight - 20);
            QString elidedText = metrics.elidedText(cellText, Qt::ElideRight, cellRect.width());
            painter.setPen(textDark);
            painter.drawText(cellRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
            
            // Vertical separator
            if (col < colCount - 1) {
                painter.setPen(QPen(borderGray, 0.8));
                painter.drawLine(xPos + scaledWidths[col], yPos + 4, xPos + scaledWidths[col], yPos + rowHeight - 4);
            }
            xPos += scaledWidths[col];
        }

        // Horizontal row border
        painter.setPen(QPen(borderGray, 0.8));
        painter.drawLine(margin, yPos + rowHeight - 1, margin + tableWidth, yPos + rowHeight - 1);
        
        yPos += rowHeight;
        rowNum++;
    }

    // Draw final table border
    painter.setPen(QPen(borderGray, 2));
    painter.drawRect(margin, tableStartY, tableWidth, yPos - tableStartY - 10);

    yPos += 80; // MUCH MORE space after table

    // ========== STATISTICS SECTION ==========
    if (yPos + 250 > pageHeight - margin) {
        pdfWriter.newPage();
        yPos = margin;
    }

    // Section header
    QRect statsSectionRect(margin, yPos, pageWidth - 2*margin, 50);
    painter.fillRect(statsSectionRect, headerBg);
    
    painter.setFont(sectionFont);
    painter.setPen(textDark);
    painter.drawText(statsSectionRect.adjusted(20, 0, -20, 0), 
                     Qt::AlignLeft | Qt::AlignVCenter, "STATISTIQUES");
    
    painter.setPen(QPen(primaryOrange, 4));
    painter.drawLine(margin + 20, yPos + 42, margin + 200, yPos + 42);
    
    yPos += 70; // MUCH MORE space after section header

    // Statistics data
    double moyenneAbonnes = totalCreateurs > 0 ? static_cast<double>(totalAbonnes) / totalCreateurs : 0.0;

    // Statistics box with border
    QRect statsBox(margin, yPos, pageWidth - 2*margin, 200);
    painter.setBrush(Qt::white);
    painter.setPen(QPen(borderGray, 2));
    painter.drawRect(statsBox);
    
    // Top accent line
    painter.setPen(QPen(primaryOrange, 5));
    painter.drawLine(margin, yPos, margin + tableWidth, yPos);

    int itemY = yPos + 40; // MORE space from top
    
    QStringList labels = {
        "Nombre total de créateurs",
        "Nombre total d'abonnés", 
        "Moyenne d'abonnés par créateur",
        "Maximum d'abonnés"
    };
    
    QStringList values = {
        QString::number(totalCreateurs),
        QString::number(totalAbonnes),
        QString::number(moyenneAbonnes, 'f', 1),
        QString::number(maxAbonnes)
    };

    painter.setFont(statsLabelFont);
    painter.setPen(textDark);
    QFontMetrics labelMetrics(statsLabelFont);
    
    for (int i = 0; i < labels.size(); ++i) {
        // Draw label
        painter.drawText(margin + 40, itemY, labels[i] + ":");
        
        // Draw value with highlight
        int labelWidth = labelMetrics.horizontalAdvance(labels[i] + ": ");
        painter.setFont(statsValueFont);
        painter.setPen(primaryOrange);
        painter.drawText(margin + 40 + labelWidth + 20, itemY, values[i]);
        
        painter.setFont(statsLabelFont);
        painter.setPen(textDark);
        itemY += 45; // MUCH MORE spacing between items
    }
    
    yPos += 210; // MUCH MORE space after statistics

    // ========== CHART SECTION ==========
    if (yPos + 500 > pageHeight - margin) {
        pdfWriter.newPage();
        yPos = margin;
    }

    // Section header
    QRect chartSectionRect(margin, yPos, pageWidth - 2*margin, 50);
    painter.fillRect(chartSectionRect, headerBg);
    
    painter.setFont(sectionFont);
    painter.setPen(textDark);
    painter.drawText(chartSectionRect.adjusted(20, 0, -20, 0), 
                     Qt::AlignLeft | Qt::AlignVCenter, "RÉPARTITION DES ABONNÉS PAR CATÉGORIE");
    
    painter.setPen(QPen(primaryOrange, 4));
    painter.drawLine(margin + 20, yPos + 42, margin + 400, yPos + 42);
    
    yPos += 70; // MUCH MORE space after section header

    // Render chart - FIXED: Proper chart rendering
    QSqlQuery chartQuery;
    chartQuery.prepare("SELECT COALESCE(CATEGORIE, 'Non classé') AS categorie, SUM(ABONNEES) AS total_abonnes FROM CREATEUR GROUP BY CATEGORIE HAVING SUM(ABONNEES) > 0 ORDER BY total_abonnes DESC");
    
    if (chartQuery.exec()) {
        auto *series = new QPieSeries();
        bool hasData = false;
        
        while (chartQuery.next()) {
            QString categorie = chartQuery.value(0).toString();
            int abonnes = chartQuery.value(1).toInt();
            
            if (abonnes > 0) {
                QPieSlice *slice = series->append(categorie, abonnes);
                slice->setLabel(QString("%1\n%2 abonnés").arg(categorie).arg(abonnes));
                slice->setLabelVisible(true);
                slice->setLabelFont(QFont("Arial", 10, QFont::Bold));
                hasData = true;
            }
        }
        
        if (hasData) {
            QChart *chart = new QChart();
            chart->addSeries(series);
            chart->setTitle("");
            chart->setBackgroundBrush(QBrush(Qt::white));
            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignRight);
            chart->legend()->setFont(QFont("Arial", 10, QFont::Normal));
            chart->legend()->setLabelColor(textDark);
            chart->setMargins(QMargins(10, 10, 10, 10));
            
            // Render chart - FIXED: Render directly to pixmap without showing widget
            const int chartSize = 450;
            const int chartX = margin + (pageWidth - 2*margin - chartSize) / 2;
            
            // Create pixmap and render chart directly to it
            QPixmap chartPixmap(chartSize, chartSize);
            chartPixmap.fill(Qt::white);
            
            QPainter chartPainter(&chartPixmap);
            chartPainter.setRenderHint(QPainter::Antialiasing);
            chartPainter.setRenderHint(QPainter::TextAntialiasing);
            
            // Create a temporary chart view for rendering (but don't show it)
            QChartView chartView(chart);
            chartView.setRenderHint(QPainter::Antialiasing);
            chartView.setRenderHint(QPainter::TextAntialiasing);
            chartView.resize(chartSize, chartSize);
            chartView.setBackgroundBrush(QBrush(Qt::white));
            
            // Render the chart view to the pixmap
            chartView.render(&chartPainter, QRect(0, 0, chartSize, chartSize));
            chartPainter.end();
            
            delete chart;
            
            if (!chartPixmap.isNull() && !chartPixmap.size().isEmpty()) {
                // Draw chart with border
                QRect chartRect(chartX - 10, yPos - 10, chartSize + 20, chartSize + 20);
                painter.setPen(QPen(borderGray, 2));
                painter.setBrush(Qt::white);
                painter.drawRect(chartRect);
                painter.drawPixmap(chartX, yPos, chartSize, chartSize, chartPixmap);
            } else {
                painter.setFont(statsLabelFont);
                painter.setPen(textDark);
                painter.drawText(QRect(margin, yPos, pageWidth - 2*margin, 200), 
                               Qt::AlignCenter, "Graphique non disponible");
            }
        } else {
            painter.setFont(statsLabelFont);
            painter.setPen(textDark);
            painter.drawText(QRect(margin, yPos, pageWidth - 2*margin, 200), 
                           Qt::AlignCenter, "Aucune donnée disponible pour le graphique");
        }
    } else {
        painter.setFont(statsLabelFont);
        painter.setPen(textDark);
        painter.drawText(QRect(margin, yPos, pageWidth - 2*margin, 200), 
                       Qt::AlignCenter, "Données du graphique non disponibles");
    }

    painter.end();
    
    QMessageBox::information(this, "Succès", 
                           QString("Rapport PDF exporté avec succès!\n\nFichier: %1\n\n%2 créateurs exportés\nTotal abonnés: %3")
                           .arg(fileName)
                           .arg(totalCreateurs)
                           .arg(totalAbonnes));
}