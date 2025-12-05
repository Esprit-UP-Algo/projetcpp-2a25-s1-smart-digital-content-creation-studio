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
#include <QPdfWriter>
#include <QPageSize>
#include <QPageLayout>
#include <QBrush>
#include <QApplication>
#include <QWidget>
#include <QTableWidget>
#include <QRandomGenerator>
#include <algorithm>

#include <cmath>
#include <QDesktopServices>
#include <QUrl>


// CONSTRUCTEUR
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
        ui->label_7->setPixmap(logo.scaled(ui->label_7->size(),
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation));

    // VALIDATEURS EMPLOYE
    {
        QRegularExpression reCin("^[0-9]{8}$");
        ui->lineEdit->setValidator(new QRegularExpressionValidator(reCin, this));
        ui->lineEdit->setMaxLength(8);
    }
    {
        QRegularExpression reNom("^[A-Za-zÀ-ÿ' -]{1,50}$");
        ui->lineEdit_2->setValidator(new QRegularExpressionValidator(reNom, this));
        ui->lineEdit_2->setMaxLength(50);
        ui->lineEdit_3->setValidator(new QRegularExpressionValidator(reNom, this));
        ui->lineEdit_3->setMaxLength(50);
    }
    {
        QRegularExpression reMail(
            R"(^(?:[A-Za-z0-9._%+\-]+)@(outlook|yahoo|gmail)\.(fr|com)$)",
            QRegularExpression::CaseInsensitiveOption);
        ui->lineEdit_4->setValidator(new QRegularExpressionValidator(reMail, this));
    }
    {
        QRegularExpression reTel("^[0-9]{8}$");
        ui->lineEdit_6->setValidator(new QRegularExpressionValidator(reTel, this));
        ui->lineEdit_6->setMaxLength(8);
    }

    // TABLE EMPLOYES
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(
        {"CIN","Nom","Prenom","Email","N Tel","Mot de passe"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    Employe::afficher(ui->tableWidget);

    // VALIDATEURS MATERIEL
    {
        QRegularExpression regexChiffres("^[0-9]+$");
        ui->lineEdit_11->setValidator(
            new QRegularExpressionValidator(regexChiffres, this));
    }
    {
        QRegularExpression regexLettres("^[a-zA-ZÀ-ÿ ]{0,20}$");
        ui->lineEdit_9->setValidator(
            new QRegularExpressionValidator(regexLettres, this));
        ui->lineEdit_9->setMaxLength(20);
        ui->lineEdit_10->setValidator(
            new QRegularExpressionValidator(regexLettres, this));
        ui->lineEdit_10->setMaxLength(20);
    }

    // TABLE MATERIEL
    ui->tableWidget_6->setColumnCount(4);
    ui->tableWidget_6->setHorizontalHeaderLabels(
        {"Type Appareil","Reference","Marque","Disponibilite"});
    ui->tableWidget_6->horizontalHeader()->setStretchLastSection(true);
    Mtmp.afficher(ui->tableWidget_6);

    // VALIDATEURS CREATEUR
    {
        QRegularExpression reCode("^\\d{1,10}$");
        ui->lineEdit_12->setValidator(new QRegularExpressionValidator(reCode, this));
        ui->lineEdit_12->setMaxLength(10);      // code unique
    }
    {
        QRegularExpression reInt("^\\d{1,10}$");
        ui->lineEdit_32->setValidator(new QRegularExpressionValidator(reInt, this));
        ui->lineEdit_32->setMaxLength(10);      // PROJETS (entier)
    }
    {
        QRegularExpression reNom("^[A-Za-zÀ-ÿ' -]{1,50}$");
        ui->lineEdit_28->setValidator(new QRegularExpressionValidator(reNom, this));
        ui->lineEdit_28->setMaxLength(50);      // NOM
        ui->lineEdit_27->setValidator(new QRegularExpressionValidator(reNom, this));
        ui->lineEdit_27->setMaxLength(50);      // PRENOM
    }
    {
        QRegularExpression reAbonnes("^\\d{1,10}$");
        ui->lineEdit_30->setValidator(new QRegularExpressionValidator(reAbonnes, this));
        ui->lineEdit_30->setMaxLength(10);      // ABONNES
    }
    // lineEdit_35 pour t_projets: texte libre
    ui->dateEdit_3->setDate(QDate::currentDate());

    // TABLE CREATEURS
    ui->tableWidget_5->setColumnCount(10);  // 10 colonnes avec T_sponsor
    ui->tableWidget_5->setHorizontalHeaderLabels(
        {"Code unique","Nom","Prenom","Date d'adhesion",
         "Categorie","Abonnes","Statut","Projets","T_projets","T_sponsor"});
    ui->tableWidget_5->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_5->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_5->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_5->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->comboBox_6->clear();
    ui->comboBox_6->addItem("Code unique", "CODE_UNIQUE");
    ui->comboBox_6->addItem("Nom", "NOM");
    ui->comboBox_6->addItem("Prénom", "PRENOM");
    ui->comboBox_6->addItem("Date d'adhésion", "DATE_ADHESION");
    ui->comboBox_6->addItem("Catégorie", "CATEGORIE");
    ui->comboBox_6->addItem("Abonnés", "ABONNEES");
    ui->comboBox_6->setCurrentIndex(0);

    refreshCreateurTable();
    ui->pushButton_19->setText("Ajouter");
    ui->pushButton_25->setText("Supprimer");
    // Chart initialization
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

    // Connect tab change
    if (ui->Widget) {
        connect(ui->Widget, &QTabWidget::currentChanged,
                this, [this](int index) {
                    if (index == 1) {
                        updateCreateurStats();
                    }
                });
    }

    // Connexions explicites
    connect(ui->tableWidget, &QTableWidget::cellClicked,
            this, &Gemploye::on_tableWidget_cellClicked);
    connect(ui->pushButton, &QPushButton::clicked,
            this, &Gemploye::on_pushButton_clicked);
    connect(ui->pushButton_20, &QPushButton::clicked,
            this, &Gemploye::on_pushButton_20_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked,
            this, &Gemploye::on_pushButton_2_clicked);
    connect(ui->tableWidget_6, &QTableWidget::cellClicked,
            this, &Gemploye::on_tableWidget_6_cellClicked);
    connect(ui->pushButton_2, &QPushButton::clicked,
            this, &Gemploye::on_pushButton_2_clicked);
    connect(ui->tableWidget_6, &QTableWidget::cellClicked,
            this, &Gemploye::on_tableWidget_6_cellClicked);

    // Connexion du bouton de paiement Stripe
    connect(ui->pushButton_26, &QPushButton::clicked,
            this, &Gemploye::on_pushButton_26_clicked);

}

Gemploye::~Gemploye()
{
    delete ui;
}

// ================= NAVIGATION =================

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

// ================= EMPLOYE =================

void Gemploye::on_pushButton_clicked()
{
    const QString cin    = ui->lineEdit->text().trimmed();
    const QString nom    = ui->lineEdit_2->text().trimmed();
    const QString prenom = ui->lineEdit_3->text().trimmed();
    const QString email  = ui->lineEdit_4->text().trimmed();
    const QString pass   = ui->lineEdit_5->text();
    const QString gsm    = ui->lineEdit_6->text().trimmed();

    if (cin.isEmpty() || nom.isEmpty() || prenom.isEmpty()
        || email.isEmpty() || pass.isEmpty() || gsm.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants",
                             "Tous les champs sont obligatoires.");
        return;
    }

    if (!Employe::cinValide(cin)) {
        QMessageBox::warning(this, "CIN invalide",
                             "Le CIN doit contenir exactement 8 chiffres.");
        return;
    }

    if (!Employe::nomPrenomValide(nom) || !Employe::nomPrenomValide(prenom)) {
        QMessageBox::warning(this, "Nom/Prenom invalides",
                             "Utilise lettres, espaces, accents, apostrophes ou tirets.");
        return;
    }

    if (!Employe::emailValide(email)) {
        QMessageBox::warning(this, "Email invalide",
                             "Autorises: outlook, yahoo ou gmail, en .fr ou .com.");
        return;
    }

    if (!Employe::telValide(gsm)) {
        QMessageBox::warning(this, "Telephone invalide",
                             "Le numero doit contenir exactement 8 chiffres.");
        return;
    }

    if (Employe::existeCin(cin)) {
        QMessageBox::warning(this, "CIN en double",
                             "Un employe avec ce CIN existe deja.");
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
        QMessageBox::critical(this, "Erreur",
                              "Insertion refusee par la base.");
    }
}

void Gemploye::on_tableWidget_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    auto *tbl = ui->tableWidget;
    if (!tbl || row < 0 || row >= tbl->rowCount()) return;

    const QString cin    = tbl->item(row, 0) ? tbl->item(row, 0)->text() : QString();
    const QString nom    = tbl->item(row, 1) ? tbl->item(row, 1)->text() : QString();
    const QString prenom = tbl->item(row, 2) ? tbl->item(row, 2)->text() : QString();
    const QString email  = tbl->item(row, 3) ? tbl->item(row, 3)->text() : QString();
    const QString gsm    = tbl->item(row, 4) ? tbl->item(row, 4)->text() : QString();
    const QString pass   = tbl->item(row, 5) ? tbl->item(row, 5)->text() : QString();

    ui->lineEdit->setText(cin);
    ui->lineEdit_2->setText(nom);
    ui->lineEdit_3->setText(prenom);
    ui->lineEdit_4->setText(email);
    ui->lineEdit_6->setText(gsm);
    ui->lineEdit_5->setText(pass);

    selectedCin = cin;
}

void Gemploye::on_pushButton_20_clicked()
{
    if (selectedCin.isEmpty()) {
        QMessageBox::warning(this, "Selection",
                             "Selectionne un employe dans le tableau.");
        return;
    }

    const QString newCin = ui->lineEdit->text().trimmed();
    const QString nom    = ui->lineEdit_2->text().trimmed();
    const QString prenom = ui->lineEdit_3->text().trimmed();
    const QString email  = ui->lineEdit_4->text().trimmed();
    const QString pass   = ui->lineEdit_5->text();
    const QString gsm    = ui->lineEdit_6->text().trimmed();

    if (newCin.isEmpty() || nom.isEmpty() || prenom.isEmpty()
        || email.isEmpty() || pass.isEmpty() || gsm.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants",
                             "Tous les champs sont obligatoires.");
        return;
    }

    if (!Employe::cinValide(newCin)) {
        QMessageBox::warning(this, "CIN invalide",
                             "Le CIN doit contenir exactement 8 chiffres.");
        return;
    }

    if (!Employe::nomPrenomValide(nom) || !Employe::nomPrenomValide(prenom)) {
        QMessageBox::warning(this, "Nom/Prenom invalides",
                             "Utilise lettres, espaces, accents, apostrophes ou tirets.");
        return;
    }

    if (!Employe::emailValide(email)) {
        QMessageBox::warning(this, "Email invalide",
                             "Autorises: outlook, yahoo ou gmail.");
        return;
    }

    if (!Employe::telValide(gsm)) {
        QMessageBox::warning(this, "Telephone invalide",
                             "Le numero doit contenir exactement 8 chiffres.");
        return;
    }

    if (newCin != selectedCin && Employe::existeCin(newCin)) {
        QMessageBox::warning(this, "Conflit CIN",
                             "Un employe avec ce CIN existe deja.");
        return;
    }

    Employe e(newCin, nom, prenom, email, pass, gsm);
    if (e.modifier(selectedCin)) {
        QMessageBox::information(this, "Succes", "Employe modifie.");
        selectedCin = newCin;
        Employe::afficher(ui->tableWidget);
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Echec de la modification.");
    }
    if (ui->tableWidget_7) {
        ui->tableWidget_7->setColumnCount(9);
        QStringList headers;
        headers << "ID créateur"
                << "Projets"
                << "T_projets"
                << "T_sponsor"         // IMPORTANT
                << "Budget projet"
                << "Budget créateur"
                << "Budget sponsor"
                << "Budget matériel"
                << "Date affectation";
        ui->tableWidget_7->setHorizontalHeaderLabels(headers);
        ui->tableWidget_7->horizontalHeader()->setStretchLastSection(true);
        ui->tableWidget_7->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidget_7->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget_7->setSelectionMode(QAbstractItemView::SingleSelection);
    }
}

void Gemploye::on_pushButton_2_clicked()
{
    if (selectedCin.isEmpty()) {
        QMessageBox::warning(this, "Selection",
                             "Selectionne un employe dans le tableau.");
        return;
    }

    const int ret = QMessageBox::question(
        this, "Supprimer",
        "Voulez-vous vraiment supprimer cet employe ?",
        QMessageBox::Yes | QMessageBox::No);
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
        QMessageBox::critical(this, "Erreur",
                              "Echec de la suppression.");
    }
}

// ================= MATERIEL =================

void Gemploye::on_pushButton_8_clicked()
{
    if (ui->lineEdit_9->text().isEmpty()
        || ui->lineEdit_10->text().isEmpty()
        || ui->lineEdit_11->text().isEmpty()) {
        QMessageBox::warning(this, "Champ vide",
                             "Veuillez remplir tous les champs.");
        return;
    }

    const QString type   = ui->lineEdit_9->text();
    const QString ref    = ui->lineEdit_11->text();
    const QString marque = ui->lineEdit_10->text();
    const QString dispo  = ui->comboBox_10->currentText();

    if (type.length() > 20 || marque.length() > 20) {
        QMessageBox::warning(this, "Erreur",
                             "Type ou marque trop long, max 20 caracteres.");
        return;
    }

    Materiel M(type, ref, marque, dispo);
    if (M.ajouter()) {
        QMessageBox::information(this, "Succes", "Materiel ajoute.");
        ui->lineEdit_9->clear();
        ui->lineEdit_10->clear();
        ui->lineEdit_11->clear();
        ui->comboBox_10->setCurrentIndex(0);
        Mtmp.afficher(ui->tableWidget_6);
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Echec de l'ajout.");
    }
}

void Gemploye::on_tableWidget_6_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    auto *tbl = ui->tableWidget_6;
    if (!tbl || row < 0 || row >= tbl->rowCount()) return;

    ui->lineEdit_9->setText(tbl->item(row, 0)->text());       // type
    ui->lineEdit_11->setText(tbl->item(row, 1)->text());      // ref
    ui->lineEdit_10->setText(tbl->item(row, 2)->text());      // marque
    selectedReference = tbl->item(row, 1)->text();

    const int idx = ui->comboBox_10->findText(tbl->item(row, 3)->text());
    if (idx != -1)
        ui->comboBox_10->setCurrentIndex(idx);
}

void Gemploye::on_pushButton_11_clicked()
{
    if (selectedReference.isEmpty()) {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez selectionner un materiel dans le tableau.");
        return;
    }

    const QString type   = ui->lineEdit_9->text();
    const QString ref    = ui->lineEdit_11->text();
    const QString marque = ui->lineEdit_10->text();
    const QString dispo  = ui->comboBox_10->currentText();

    if (type.isEmpty() || marque.isEmpty() || ref.isEmpty()) {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez remplir tous les champs.");
        return;
    }

    Materiel M(type, ref, marque, dispo);
    if (M.modifier(selectedReference)) {
        QMessageBox::information(this, "Succes", "Materiel modifie.");
        selectedReference = ref;
        Mtmp.afficher(ui->tableWidget_6);
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Echec de la modification.");
    }
}

void Gemploye::on_pushButton_9_clicked()
{
    if (selectedReference.isEmpty()) {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez selectionner un materiel a supprimer.");
        return;
    }

    const int ret = QMessageBox::question(
        this, "Supprimer",
        "Voulez-vous vraiment supprimer ce materiel ?",
        QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    Materiel M;
    if (M.supprimer(selectedReference)) {
        QMessageBox::information(this, "Succes", "Materiel supprime.");
        selectedReference.clear();
        Mtmp.afficher(ui->tableWidget_6);
        ui->lineEdit_9->clear();
        ui->lineEdit_10->clear();
        ui->lineEdit_11->clear();
        ui->comboBox_10->setCurrentIndex(0);
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Echec de la suppression.");
    }
}

// ================= CREATEUR – HELPERS =================


void Gemploye::clearCreateurForm()
{
    selectedCreateurId = -1;
    ui->lineEdit_12->clear();   // code unique
    ui->lineEdit_28->clear();   // nom
    ui->lineEdit_27->clear();   // prenom
    ui->comboBox_8->setCurrentIndex(-1);
    ui->lineEdit_30->clear();   // abonnes
    ui->lineEdit_32->clear();   // projets
    if (ui->lineEdit_35)
        ui->lineEdit_35->clear(); // t_projets
    if (ui->lineEdit_37)
        ui->lineEdit_37->clear(); // t_sponsor <<<< IMPORTANT
    ui->dateEdit_3->setDate(QDate::currentDate());
    ui->tableWidget_5->clearSelection();
    ui->pushButton_19->setText("Ajouter");
}


void Gemploye::refreshCreateurTable()
{
    if (!ui->tableWidget_5) return;

    const QString search = ui->lineEdit_31
                               ? ui->lineEdit_31->text().trimmed()
                               : QString();
    QString orderField = "CODE_UNIQUE";

    if (ui->comboBox_6 && ui->comboBox_6->currentIndex() >= 0) {
        QString f = ui->comboBox_6->currentData().toString();
        if (!f.isEmpty())
            orderField = f;
    }

    // REQUÊTE AVEC T_SPONSOR
    QString queryStr = R"(
        SELECT ID_CREATEUR, CODE_UNIQUE, NOM, PRENOM, DATE_ADHESION,
               CATEGORIE, ABONNEES, PROJETS, T_PROJETS, T_SPONSOR
        FROM CREATEUR
    )";

    if (!search.isEmpty()) {
        queryStr += " WHERE NOM LIKE '%" + search + "%'"
                                                    " OR PRENOM LIKE '%" + search + "%'";
    }

    queryStr += " ORDER BY " + orderField;

    ui->tableWidget_5->setRowCount(0);
    ui->tableWidget_5->setColumnCount(10);  // 10 COLONNES
    ui->tableWidget_5->setHorizontalHeaderLabels(
        {"Code unique","Nom","Prenom","Date d'adhesion",
         "Categorie","Abonnes","Statut","Projets","T_projets","T_sponsor"});

    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qWarning() << "refreshCreateurTable error:"
                   << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->tableWidget_5->insertRow(row);

        // INDICES DES COLONNES SQL:
        // 0 = ID_CREATEUR
        // 1 = CODE_UNIQUE
        // 2 = NOM
        // 3 = PRENOM
        // 4 = DATE_ADHESION
        // 5 = CATEGORIE
        // 6 = ABONNEES
        // 7 = PROJETS
        // 8 = T_PROJETS
        // 9 = T_SPONSOR

        int id      = query.value(0).toInt();
        int abonnes = query.value(6).toInt();

        // Colonne 0: Code unique (avec ID en UserRole)
        auto *codeItem = new QTableWidgetItem(query.value(1).toString());
        codeItem->setData(Qt::UserRole, id);
        ui->tableWidget_5->setItem(row, 0, codeItem);

        // Colonne 1: NOM
        ui->tableWidget_5->setItem(row, 1,
                                   new QTableWidgetItem(query.value(2).toString()));

        // Colonne 2: PRENOM
        ui->tableWidget_5->setItem(row, 2,
                                   new QTableWidgetItem(query.value(3).toString()));

        // Colonne 3: DATE (formatée)
        ui->tableWidget_5->setItem(row, 3,
                                   new QTableWidgetItem(query.value(4).toDate()
                                                            .toString("dd/MM/yyyy")));

        // Colonne 4: CATEGORIE
        ui->tableWidget_5->setItem(row, 4,
                                   new QTableWidgetItem(query.value(5).toString()));

        // Colonne 5: ABONNES
        ui->tableWidget_5->setItem(row, 5,
                                   new QTableWidgetItem(QString::number(abonnes)));

        // Colonne 6: STATUT (coloré)
        auto *statusItem =
            new QTableWidgetItem(Createur::getStatusText(abonnes));
        statusItem->setBackground(Createur::getStatusColor(abonnes));
        statusItem->setForeground(Qt::white);
        statusItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_5->setItem(row, 6, statusItem);

        // Colonne 7: PROJETS
        ui->tableWidget_5->setItem(row, 7,
                                   new QTableWidgetItem(query.value(7).toString()));

        // Colonne 8: T_PROJETS
        ui->tableWidget_5->setItem(row, 8,
                                   new QTableWidgetItem(query.value(8).toString()));

        // Colonne 9: T_SPONSOR <<<< IMPORTANT
        ui->tableWidget_5->setItem(row, 9,
                                   new QTableWidgetItem(query.value(9).toString()));

        row++;
    }

    updateCreateurStats();
}


void Gemploye::on_tableWidget_5_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    auto *tbl = ui->tableWidget_5;
    if (!tbl || row < 0 || row >= tbl->rowCount()) return;

    auto *codeItem = tbl->item(row, 0);
    if (!codeItem) return;

    selectedCreateurId = codeItem->data(Qt::UserRole).toInt();

    // Code unique
    ui->lineEdit_12->setText(codeItem->text());

    // Nom
    ui->lineEdit_28->setText(tbl->item(row, 1)
                                 ? tbl->item(row, 1)->text() : QString());

    // Prénom
    ui->lineEdit_27->setText(tbl->item(row, 2)
                                 ? tbl->item(row, 2)->text() : QString());

    // Date d'adhésion
    const QString dateStr = tbl->item(row, 3)
                                ? tbl->item(row, 3)->text()
                                : QString();
    const QDate date = QDate::fromString(dateStr, "dd/MM/yyyy");
    if (date.isValid())
        ui->dateEdit_3->setDate(date);

    // Catégorie
    ui->comboBox_8->setCurrentText(tbl->item(row, 4)
                                       ? tbl->item(row, 4)->text()
                                       : QString());

    // Abonnés
    ui->lineEdit_30->setText(tbl->item(row, 5)
                                 ? tbl->item(row, 5)->text()
                                 : QString());

    // Projets (colonne 7)
    if (tbl->columnCount() > 7 && ui->lineEdit_32)
        ui->lineEdit_32->setText(tbl->item(row, 7)
                                     ? tbl->item(row, 7)->text()
                                     : QString());

    // T_projets (colonne 8)
    if (tbl->columnCount() > 8 && ui->lineEdit_35)
        ui->lineEdit_35->setText(tbl->item(row, 8)
                                     ? tbl->item(row, 8)->text()
                                     : QString());

    // T_sponsor (colonne 9) <<<< IMPORTANT
    if (tbl->columnCount() > 9 && ui->lineEdit_37)
        ui->lineEdit_37->setText(tbl->item(row, 9)
                                     ? tbl->item(row, 9)->text()
                                     : QString());

    ui->pushButton_19->setText("Mettre a jour");
}


void Gemploye::on_pushButton_19_clicked()
{
    const QString codeText    = ui->lineEdit_12->text().trimmed();
    const QString nom         = ui->lineEdit_28->text().trimmed();
    const QString prenom      = ui->lineEdit_27->text().trimmed();
    const QString categorie   = ui->comboBox_8->currentText().trimmed();
    const QString abonnesText = ui->lineEdit_30->text().trimmed();
    const QString projetsText = ui->lineEdit_32->text().trimmed();
    const QString t_projets    = ui->lineEdit_35
                                 ? ui->lineEdit_35->text().trimmed()
                                 : QString();
    const QString t_sponsor   = ui->lineEdit_37  // <<<< RÉCUPÉRÉ ICI
                                  ? ui->lineEdit_37->text().trimmed()
                                  : QString();
    const QDate dateAdhesion  = ui->dateEdit_3->date();

    if (codeText.isEmpty() || nom.isEmpty() || prenom.isEmpty()
        || abonnesText.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants",
                             "Code, nom, prenom et abonnes sont obligatoires.");
        return;
    }

    if (!Createur::codeUniqueValide(codeText)) {
        QMessageBox::warning(this, "Code invalide",
                             "Le code unique doit contenir uniquement des chiffres (max 10).");
        return;
    }

    if (!Createur::nomPrenomValide(nom)
        || !Createur::nomPrenomValide(prenom)) {
        QMessageBox::warning(this, "Nom/Prenom invalides",
                             "Utilise lettres, espaces, accents, apostrophes ou tirets.");
        return;
    }

    if (!Createur::categorieValide(categorie)) {
        QMessageBox::warning(this, "Categorie invalide",
                             "La categorie ne doit contenir que lettres/chiffres simples.");
        return;
    }

    if (!Createur::abonnesValide(abonnesText)) {
        QMessageBox::warning(this, "Abonnes invalides",
                             "Le nombre d'abonnes doit etre un entier positif (max 10 chiffres).");
        return;
    }

    int abonnes = abonnesText.toInt();
    int projets = 0;

    if (!projetsText.isEmpty()) {
        if (!Createur::abonnesValide(projetsText)) {
            QMessageBox::warning(this, "Projets invalides",
                                 "Le nombre de projets doit etre un entier positif.");
            return;
        }
        projets = projetsText.toInt();
    }

    int codeUnique = codeText.toInt();
    bool isEdit    = (selectedCreateurId >= 0);

    if (!isEdit && Createur::codeUniqueExiste(codeUnique)) {
        QMessageBox::warning(this, "Code en double",
                             "Un createur avec ce code unique existe deja.");
        return;
    }

    if (isEdit && Createur::codeUniqueExiste(codeUnique, selectedCreateurId)) {
        QMessageBox::warning(this, "Conflit code",
                             "Ce code unique est deja attribue a un autre createur.");
        return;
    }

    // CRÉER L'OBJET AVEC T_SPONSOR <<<< IMPORTANT
    Createur c(codeUnique, nom, prenom, dateAdhesion,
               categorie, abonnes, projets, t_projets, t_sponsor);

    if (isEdit)
        c.setId(selectedCreateurId);

    bool ok = isEdit ? c.modifier() : c.ajouter();
    if (!ok) {
        QMessageBox::critical(this, "Erreur",
                              isEdit ? "La modification a echoue."
                                     : "L'ajout a echoue.");
        return;
    }

    QMessageBox::information(this, "Succes",
                             isEdit ? "Createur modifie."
                                    : "Createur ajoute.");
    clearCreateurForm();
    refreshCreateurTable();  // <<<< RAFRAÎCHIT LE TABLEAU AVEC T_SPONSOR
}

void Gemploye::on_pushButton_25_clicked()
{
    if (selectedCreateurId < 0) {
        QMessageBox::warning(this, "Selection",
                             "Selectionne un createur dans le tableau.");
        return;
    }

    const int ret = QMessageBox::question(
        this, "Supprimer",
        "Voulez-vous vraiment supprimer ce createur ?",
        QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    if (Createur::supprimer(selectedCreateurId)) {
        QMessageBox::information(this, "Succes", "Createur supprime.");
        clearCreateurForm();
        refreshCreateurTable();
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Echec de la suppression.");
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

// ================= STATS CREATEUR =================

void Gemploye::updateCreateurStats()
{
    if (!createurChartView) return;

    QSqlQuery query;
    query.prepare(R"(
        SELECT COALESCE(CATEGORIE, 'Non classé') AS categorie,
               SUM(ABONNEES) AS total_abonnes
        FROM CREATEUR
        GROUP BY COALESCE(CATEGORIE, 'Non classé')
        ORDER BY total_abonnes DESC
    )");

    if (!query.exec()) {
        qWarning() << "updateCreateurStats error:" << query.lastError().text();
        return;
    }

    auto *series = new QPieSeries();
    int sliceCount = 0;
    double others = 0.0;
    double totalAbonnes = 0.0;

    // 1) lire toutes les catégories et calculer le total
    QList<QPair<QString, double>> dataList;
    while (query.next()) {
        const QString categorie = query.value(0).toString();
        const double total = query.value(1).toDouble();
        if (total <= 0) continue;

        totalAbonnes += total;
        dataList.append(qMakePair(categorie, total));
    }

    // 2) créer les slices, garder max 5 catégories, reste = "Autres"
    for (int i = 0; i < dataList.size(); ++i) {
        const QString categorie = dataList[i].first;
        const double total = dataList[i].second;

        if (sliceCount < 5) {
            double percentage = (totalAbonnes > 0)
            ? (total / totalAbonnes) * 100.0
            : 0.0;

            // texte pour la LÉGENDE
            QString label = QString("%1 %2 (%3%)")
                                .arg(categorie)
                                .arg(static_cast<long long>(total))
                                .arg(percentage, 0, 'f', 1);

            auto *slice = series->append(label, total);
            slice->setLabelVisible(false);      // pas de texte sur le camembert
            sliceCount++;
        } else {
            others += total;
        }
    }

    if (others > 0.0) {
        double othersPercentage = (totalAbonnes > 0)
        ? (others / totalAbonnes) * 100.0
        : 0.0;

        QString label = QString("Autres %1 (%2%)")
                            .arg(static_cast<long long>(others))
                            .arg(othersPercentage, 0, 'f', 1);

        auto *slice = series->append(label, others);
        slice->setLabelVisible(false);
    }

    // si vraiment aucune donnée
    if (series->slices().isEmpty())
        series->append("Aucun abonné", 1.0);

    series->setLabelsVisible(false);

    auto *chart = new QChart();
    chart->setTitle("Répartition des abonnés par catégorie");
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // légende lisible sur le côté
    QLegend *legend = chart->legend();
    legend->setVisible(true);
    legend->setAlignment(Qt::AlignRight);
    legend->setMarkerShape(QLegend::MarkerShapeRectangle);

    QFont legendFont = legend->font();
    legendFont.setPointSize(9);
    legend->setFont(legendFont);

    createurChartView->setChart(chart);
    createurChartView->setRenderHint(QPainter::Antialiasing);
}
// ================= CREATEUR – PDF AFFECTATION =================
// DANS gemploye.cpp - Remplacer COMPLÈTEMENT on_pushButton_22_clicked()

void Gemploye::on_pushButton_22_clicked()
{
    if (selectedCreateurId < 0) {
        QMessageBox::warning(this, "Affectation",
                             "Sélectionne d'abord un créateur dans le tableau.");
        return;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT CODE_UNIQUE, NOM, PRENOM, PROJETS, T_PROJETS, T_SPONSOR, DATE_ADHESION
        FROM CREATEUR
        WHERE ID_CREATEUR = :id
    )");
    query.bindValue(":id", selectedCreateurId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur base",
                              "Impossible de récupérer les données du créateur.\n\n"
                                  + query.lastError().text());
        return;
    }

    if (!query.next()) {
        QMessageBox::warning(this, "Affectation",
                             "Ce créateur n'existe plus en base.");
        return;
    }

    int code           = query.value(0).toInt();
    QString nom        = query.value(1).toString();
    QString prenom     = query.value(2).toString();
    int projets        = query.value(3).toInt();
    QString t_projets   = query.value(4).toString();
    QString t_sponsor  = query.value(5).toString();
    QDate dateAdhesion = query.value(6).toDate();

    Q_UNUSED(code);
    Q_UNUSED(nom);
    Q_UNUSED(prenom);
    Q_UNUSED(dateAdhesion);

    // Calcul des budgets
    double totalBudget = 0.0;
    if (projets <= 0) {
        totalBudget = 300.0;
    } else {
        totalBudget = projets * 100.0;
    }

    auto *rng = QRandomGenerator::global();
    int r1 = rng->bounded(1, 11);
    int r2 = rng->bounded(1, 11);
    int r3 = rng->bounded(1, 11);

    QVector<double> frac;
    frac << static_cast<double>(r1)
         << static_cast<double>(r2)
         << static_cast<double>(r3);

    std::sort(frac.begin(), frac.end());

    double sumFrac = frac[0] + frac[1] + frac[2];
    if (sumFrac <= 0.0) {
        frac[0] = 1.0;
        frac[1] = 2.0;
        frac[2] = 3.0;
        sumFrac = 6.0;
    }

    double factor = totalBudget / sumFrac;

    double budgetMat       = frac[0] * factor;
    double budgetSponsor   = frac[1] * factor;
    double budgetCreateur  = frac[2] * factor;

    auto round2 = [](double v) {
        return std::round(v * 100.0) / 100.0;
    };

    budgetMat       = round2(budgetMat);
    budgetSponsor   = round2(budgetSponsor);
    budgetCreateur  = round2(budgetCreateur);
    totalBudget     = round2(totalBudget);

    double sumParts = budgetMat + budgetSponsor + budgetCreateur;
    double delta    = round2(totalBudget - sumParts);
    budgetCreateur  = round2(budgetCreateur + delta);

    // Mise à jour base
    {
        QSqlQuery qUpdate;
        qUpdate.prepare(R"(
            UPDATE CREATEUR SET
                BUDGET_CREATEUR = :bc,
                BUDGET_SPONSOR  = :bs,
                BUDGET_MATERIEL = :bm,
                BUDGET_PROJETS   = :bt
            WHERE ID_CREATEUR = :id
        )");
        qUpdate.bindValue(":bc", budgetCreateur);
        qUpdate.bindValue(":bs", budgetSponsor);
        qUpdate.bindValue(":bm", budgetMat);
        qUpdate.bindValue(":bt", totalBudget);
        qUpdate.bindValue(":id", selectedCreateurId);
        if (!qUpdate.exec()) {
            qWarning() << "Update budgets error:" << qUpdate.lastError().text();
        }
    }

    if (!ui->tableWidget_7) {
        QMessageBox::warning(this, "Affectation",
                             "tableWidget_7 n'existe pas dans l'interface.");
        return;
    }

    QTableWidget *tw = ui->tableWidget_7;

    // 8 COLONNES (SANS PROJETS)
    if (tw->columnCount() != 8) {
        tw->setColumnCount(8);
        QStringList headers;
        headers << "ID créateur"
                << "T_projets"
                << "T_sponsor"
                << "Budget projet"
                << "Budget créateur"
                << "Budget sponsor"
                << "Budget matériel"
                << "Date affectation";
        tw->setHorizontalHeaderLabels(headers);
        tw->horizontalHeader()->setStretchLastSection(true);
        tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tw->setSelectionBehavior(QAbstractItemView::SelectRows);
        tw->setSelectionMode(QAbstractItemView::SingleSelection);
    }

    int row = tw->rowCount();
    tw->insertRow(row);

    // NOUVELLE STRUCTURE : 8 colonnes
    tw->setItem(row, 0, new QTableWidgetItem(QString::number(selectedCreateurId)));
    tw->setItem(row, 1, new QTableWidgetItem(t_projets));       // Col 1 maintenant
    tw->setItem(row, 2, new QTableWidgetItem(t_sponsor));      // Col 2
    tw->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(totalBudget, 0, 'f', 2)));
    tw->setItem(row, 4, new QTableWidgetItem(QString("$%1").arg(budgetCreateur, 0, 'f', 2)));
    tw->setItem(row, 5, new QTableWidgetItem(QString("$%1").arg(budgetSponsor, 0, 'f', 2)));
    tw->setItem(row, 6, new QTableWidgetItem(QString("$%1").arg(budgetMat, 0, 'f', 2)));
    tw->setItem(row, 7, new QTableWidgetItem(
                            QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss")));

    tw->resizeColumnsToContents();

    QMessageBox::information(this, "Succès",
                             "Affectation enregistrée et budgets distribués.");
}


// ================= 3) FONCTION on_pushButton_24_clicked - PDF AMÉLIORÉ ET COLORÉ =================

void Gemploye::on_pushButton_24_clicked()
{
    if (!ui->tableWidget_7) {
        QMessageBox::warning(this, "Export PDF",
                             "tableWidget_7 n'existe pas dans l'interface.");
        return;
    }

    QTableWidget *tw = ui->tableWidget_7;

    if (tw->rowCount() == 0) {
        QMessageBox::warning(this, "Export PDF",
                             "Aucune donnée à exporter.\nCrée d'abord des affectations.");
        return;
    }

    QString basePath = "C:/Users/siwar/OneDrive/Desktop/siwar";
    QDir dir(basePath);
    if (!dir.exists()) {
        QMessageBox::critical(this, "Export PDF",
                              "Le dossier n'existe pas :\n" + basePath);
        return;
    }

    QString fileName = dir.filePath("Historique_Affectations.pdf");

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);
    pdfWriter.setResolution(300);

    QPainter painter(&pdfWriter);
    if (!painter.isActive()) {
        QMessageBox::critical(this, "Export PDF",
                              "Impossible de créer le PDF :\n" + fileName);
        return;
    }

    const int pageWidth    = pdfWriter.width();
    const int pageHeight   = pdfWriter.height();
    const int margin       = 60;
    const int bottomMargin = 100;

    int yPos = margin;

    // COULEURS STYLE MODERNE
    QColor headerBgColor(199, 21, 133);      // Rose fuchsia (comme l'exemple)
    QColor headerTextColor(Qt::white);
    QColor rowEvenColor(240, 240, 240);      // Gris clair
    QColor rowOddColor(Qt::white);
    QColor borderColor(180, 180, 180);       // Gris bordure

    // ============ TITRE ============
    QFont titleFont("Arial", 20, QFont::Bold);
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    painter.drawText(QRect(margin, yPos, pageWidth - 2*margin, 50),
                     Qt::AlignCenter,
                     "Historique des affectations");
    yPos += 60;


    // ============ TABLEAU ============
    int colCount = tw->columnCount();
    if (colCount <= 0) return;

    int tableWidth = pageWidth - 2 * margin;
    int colWidth   = tableWidth / colCount;

    auto drawTableHeader = [&](int &y) {
        QFont headerFont("Arial", 10, QFont::Bold);
        painter.setFont(headerFont);

        int xPos = margin;
        const int headerHeight = 40;

        for (int col = 0; col < colCount; ++col) {
            QRect rect(xPos, y, colWidth, headerHeight);

            // Fond coloré
            painter.fillRect(rect, headerBgColor);

            // Bordure
            painter.setPen(QPen(borderColor, 2));
            painter.drawRect(rect);

            // Texte blanc
            painter.setPen(headerTextColor);
            QString headerText;
            QTableWidgetItem *hItem = tw->horizontalHeaderItem(col);
            headerText = hItem ? hItem->text() : QString("Col %1").arg(col + 1);

            painter.drawText(rect.adjusted(5, 0, -5, 0),
                             Qt::AlignVCenter | Qt::AlignCenter,
                             headerText);

            xPos += colWidth;
        }
        y += headerHeight;
    };

    drawTableHeader(yPos);

    // ============ LIGNES DU TABLEAU ============
    QFont cellFont("Arial", 9);
    painter.setFont(cellFont);

    const int rowHeight = 35;

    for (int row = 0; row < tw->rowCount(); ++row) {
        // Nouvelle page si nécessaire
        if (yPos + rowHeight + bottomMargin > pageHeight) {
            pdfWriter.newPage();
            yPos = margin;

            // Redessiner le header
            QFont titleFont2("Arial", 16, QFont::Bold);
            painter.setFont(titleFont2);
            painter.setPen(Qt::black);
            painter.drawText(QRect(margin, yPos, pageWidth - 2*margin, 40),
                             Qt::AlignCenter,
                             "Historique des affectations");
            yPos += 50;

            drawTableHeader(yPos);
            painter.setFont(cellFont);
        }

        // Alternance des couleurs de lignes
        QColor rowBgColor = (row % 2 == 0) ? rowEvenColor : rowOddColor;

        int xPos = margin;
        for (int col = 0; col < colCount; ++col) {
            QString text;
            QTableWidgetItem *item = tw->item(row, col);
            if (item)
                text = item->text();

            QRect rect(xPos, yPos, colWidth, rowHeight);

            // Fond coloré alternance
            painter.fillRect(rect, rowBgColor);

            // Bordure
            painter.setPen(QPen(borderColor, 1));
            painter.drawRect(rect);

            // Texte noir centré
            painter.setPen(Qt::black);
            painter.drawText(rect.adjusted(5, 0, -5, 0),
                             Qt::AlignVCenter | Qt::AlignCenter,
                             text);

            xPos += colWidth;
        }

        yPos += rowHeight;
    }

    painter.end();

    QMessageBox::information(this, "Export PDF",
                             "PDF généré avec succès.\n\nChemin :\n" + fileName);
}
void Gemploye::on_pushButton_26_clicked()
{
    if (selectedCreateurId < 0) {
        QMessageBox::warning(this, "Paiement",
                             "Veuillez d'abord sélectionner un créateur.");
        return;
    }

    int ret = QMessageBox::question(
        this,
        "Confirmation de paiement",
        "Vous allez être redirigé vers la page de paiement Stripe.\n"
        "Voulez-vous continuer ?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (ret == QMessageBox::Yes) {
        QUrl stripeUrl("https://buy.stripe.com/test_28E5kDh0Tdn4bok4Br1ck02");

        if (!QDesktopServices::openUrl(stripeUrl)) {
            QMessageBox::warning(this, "Erreur",
                                 "Impossible d'ouvrir le lien de paiement.");
        }
    }
}
