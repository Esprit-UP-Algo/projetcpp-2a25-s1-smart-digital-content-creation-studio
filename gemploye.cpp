#include "gemploye.h"
#include "ui_gemploye.h"
#include "employe.h"
#include <QMessageBox>

Gemploye::Gemploye(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gemploye)
{
    ui->setupUi(this);
    // Chargement initial du modèle dès le démarrage
    QSqlQueryModel *model = e_global.afficher();
    ui->tableemp->setModel(model);

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

    //controle de saisie

    QRegularExpression cinRegex("^[0-9]{6,12}$");  // CIN : chiffres 6-12
    QRegularExpression nameRegex("^[A-Za-zÀ-ÖØ-öø-ÿ\\s\\-']{2,50}$"); // nom/prénom
    QRegularExpression emailRegex("^[\\w\\.\\-]+@[\\w\\-]+\\.[A-Za-z]{2,6}$"); // email basique
    QRegularExpression telRegex("^\\+?[0-9]{7,15}$"); // téléphone
    QRegularExpression mdpRegex("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)(?=.*[^A-Za-z0-9]).{8,}$"); // mot de passe fort

    ui->linecin->setValidator(new QRegularExpressionValidator(cinRegex, this));
    ui->linenom->setValidator(new QRegularExpressionValidator(nameRegex, this));
    ui->lineprenom->setValidator(new QRegularExpressionValidator(nameRegex, this));
    ui->lineemail->setPlaceholderText("exemple@domaine.com");
    ui->linentel->setValidator(new QRegularExpressionValidator(telRegex, this));
    ui->linemdp->setPlaceholderText("8+ caractères, maj, min, chiffre, symbole");
    ui->linemdp->setEchoMode(QLineEdit::Password);

    // Vérification au clic sur "ajouter"
    connect(ui->ajouter, &QPushButton::clicked, [=]() {
        QStringList erreurs;

        QString cin = ui->linecin->text();
        QString nom = ui->linenom->text();
        QString prenom = ui->lineprenom->text();
        QString email = ui->lineemail->text();
        QString tel = ui->linentel->text();
        QString mdp = ui->linemdp->text();

        if (!cinRegex.match(cin).hasMatch()) erreurs << "CIN invalide (6 à 12 chiffres).";
        if (!nameRegex.match(nom).hasMatch()) erreurs << "Nom invalide (lettres uniquement).";
        if (!nameRegex.match(prenom).hasMatch()) erreurs << "Prénom invalide (lettres uniquement).";
        if (!emailRegex.match(email).hasMatch()) erreurs << "Email invalide.";
        if (!telRegex.match(tel).hasMatch()) erreurs << "Numéro de téléphone invalide.";
        if (!mdpRegex.match(mdp).hasMatch()) erreurs << "Mot de passe invalide (8+ chars, maj, min, chiffre, symbole).";

        if (!erreurs.isEmpty()) {
            QMessageBox::warning(this, "Erreurs de saisie", erreurs.join("\n"));
            return;
        }    });



}

Gemploye::~Gemploye()
{
    delete ui;
}

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







void Gemploye::on_ajouter_clicked()
{
    int cin = ui->linecin->text().toInt();
    QString nom = ui->linenom->text();
    QString prenom = ui->lineprenom->text();
    QString email = ui->lineemail->text();
    int n_tel = ui->linentel->text().toInt();
    QString mdp = ui->linemdp->text();

    employee e(cin, nom, prenom, email, n_tel, mdp);

    if(e.ajouter()) {
        ui->tableemp->setModel(e_global.afficher());
        QMessageBox::information(this, "OK", "Ajout effectué");
    } else {
        QMessageBox::critical(this, "Erreur", "Ajout non effectué");
    }
}




void Gemploye::on_modifier_clicked()
{
    if(selectedId == 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un employé");
        return;
    }

    int cin = ui->linecin->text().toInt();
    QString nom = ui->linenom->text();
    QString prenom = ui->lineprenom->text();
    QString email = ui->lineemail->text();
    int n_tel = ui->linentel->text().toInt();
    QString mdp = ui->linemdp->text();

    if(e_global.modifier(selectedId, cin, nom, prenom, email, n_tel, mdp)) {
        ui->tableemp->setModel(e_global.afficher());
        QMessageBox::information(this, "OK", "Modification effectuée");
    } else {
        QMessageBox::critical(this, "Erreur", "Modification échouée");
    }
}


void Gemploye::on_tableemp_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int row = index.row(); // ligne sélectionnée
    QAbstractItemModel *model = ui->tableemp->model();

    // Récupération des valeurs de la ligne cliquée
    selectedId = model->data(model->index(row, 0)).toInt(); // ID_EMPLOYE (assure-toi qu’il est en colonne 0)
    ui->linecin->setText(model->data(model->index(row, 1)).toString());
    ui->linenom->setText(model->data(model->index(row, 2)).toString());
    ui->lineprenom->setText(model->data(model->index(row, 3)).toString());
    ui->lineemail->setText(model->data(model->index(row, 4)).toString());
    ui->linentel->setText(model->data(model->index(row, 5)).toString());
    ui->linemdp->setText(model->data(model->index(row, 6)).toString());
}


void Gemploye::on_supprimer_clicked()
{
    int cin = ui->linecin->text().toInt(); // lineCINsup est le QLineEdit où tu saisis le CIN

    if(cin == 0) {
        QMessageBox::warning(this, "Attention", "Veuillez entrer un numéro de CIN valide !");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation",
                                  "Voulez-vous vraiment supprimer cet employé ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (e_global.supprimer(cin)) {
            ui->tableemp->setModel(e_global.afficher());
            QMessageBox::information(this, "Succès", "Employé supprimé avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur", "Suppression échouée. Vérifiez le CIN.");
        }
    }
}

