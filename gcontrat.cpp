#include "gcontrat.h"
#include "ui_gcontrat.h"
#include "connection.h"

#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>

#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QChart>
#include <QLegend>
#include <QLocale>


Gcontrat::Gcontrat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gcontrat)
{
    ui->setupUi(this);

    connect(ui->lineEdit_19, &QLineEdit::returnPressed, this, &Gcontrat::rechercherContratParType);
    connect(ui->comboBox_3, &QComboBox::currentIndexChanged, this, &Gcontrat::trierContratsParPrix);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &Gcontrat::exporterPDF);

    ui->lineEdit_8->setValidator(new QRegularExpressionValidator(QRegularExpression("^[A-Za-z ]+$"), this));
    ui->lineEdit_17->setValidator(new QIntValidator(1, 999999, this));
    ui->lineEdit_35->setValidator(new QIntValidator(1, 999999, this));
    ui->lineEdit_37->setValidator(new QIntValidator(1, 999999, this));

    ui->label_alert->installEventFilter(this);
    // Configuration du System Tray Icon
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/alert.png")); // Mettez votre icône d'application
    trayIcon->setVisible(true);
    trayIcon->setToolTip("Gestion des Contrats");

    // Connexion du clic sur la notification
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Gcontrat::showAlertContracts);

    // Timer pour vérifier les alertes toutes les heures
    alertTimer = new QTimer(this);
    connect(alertTimer, &QTimer::timeout, this, &Gcontrat::checkContractAlerts);
    alertTimer->start(3600000); // 3600000 ms = 1 heure

    // Vérification immédiate au démarrage
    checkContractAlerts();

    afficherContrats();

    createFloatingChatbot();

}

// ==================== MODE INTÉGRÉ ====================

void Gcontrat::setEmbedded(bool embedded)
{
    m_embedded = embedded;
    if (m_embedded) {
        applyEmbeddedLayout();
    }
}

void Gcontrat::applyEmbeddedLayout()
{
    // Cacher la barre latérale interne de Gcontrat (doublon avec la barre principale)
    if (ui->frame) {
        ui->frame->hide();
    }

    // Étendre le contenu principal
    if (ui->stackedWidget && ui->centralwidget) {
        ui->stackedWidget->setGeometry(ui->centralwidget->rect());
    }

    // Pas de bouton flottant/chatbot en mode intégré
    if (chatbotFloatingButton) chatbotFloatingButton->hide();
    if (chatbotWindow) {
        chatbotWindow->hide();
        chatbotVisible = false;
    }
}

Gcontrat::~Gcontrat()
{
    if (alertTimer) {
        alertTimer->stop();
        delete alertTimer;
    }
    if (trayIcon) {
        trayIcon->hide();
        delete trayIcon;
    }

    if (chatbotWindow) {
        delete chatbotWindow;
    }
    if (chatbotFloatingButton) {
        delete chatbotFloatingButton;
    }
    delete ui;
}

// ========== NAVIGATION ==========
void Gcontrat::on_Employ_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    if (chatbotFloatingButton) {
        chatbotFloatingButton->hide();
    }
    if (chatbotWindow && chatbotVisible) {
        chatbotWindow->hide();
        chatbotVisible = false;
    }
}

void Gcontrat::on_Employ_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    if (chatbotFloatingButton) {
        chatbotFloatingButton->hide();
    }
    if (chatbotWindow && chatbotVisible) {
        chatbotWindow->hide();
        chatbotVisible = false;
    }
}

void Gcontrat::on_Employ_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    afficherContrats(); // Actualiser l'affichage quand on arrive sur la page contrats
    // Afficher le bouton chatbot uniquement sur cette page
        if (chatbotFloatingButton) {
        chatbotFloatingButton->show();
        chatbotFloatingButton->raise();
    }
}

void Gcontrat::on_Employ_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    if (chatbotFloatingButton) {
        chatbotFloatingButton->hide();
    }
    if (chatbotWindow && chatbotVisible) {
        chatbotWindow->hide();
        chatbotVisible = false;
    }
}

void Gcontrat::on_Employ_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    if (chatbotFloatingButton) {
        chatbotFloatingButton->hide();
    }
    if (chatbotWindow && chatbotVisible) {
        chatbotWindow->hide();
        chatbotVisible = false;
    }
}

void Gcontrat::on_Employ_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    if (chatbotFloatingButton) {
        chatbotFloatingButton->hide();
    }
    if (chatbotWindow && chatbotVisible) {
        chatbotWindow->hide();
        chatbotVisible = false;
    }
}

// ========== GESTION DES CONTRATS ==========

bool Gcontrat::sponsorExists(int id)
{
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM SMART_CONTENT.SPONSOR WHERE ID_SPONSOR = :id");
    q.bindValue(":id", id);
    if(!q.exec()) { qDebug() << q.lastError().text(); return false; }
    if(q.next()) return q.value(0).toInt() > 0;
    return false;
}

bool Gcontrat::createurExists(int id)
{
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM SMART_CONTENT.CREATEUR WHERE ID_CREATEUR = :id");
    q.bindValue(":id", id);
    if(!q.exec()) { qDebug() << q.lastError().text(); return false; }
    if(q.next()) return q.value(0).toInt() > 0;
    return false;
}

void Gcontrat::clearForm()
{
    ui->lineEdit_8->clear();
    ui->lineEdit_17->clear();
    ui->lineEdit_18->clear();
    ui->lineEdit_35->clear();
    ui->lineEdit_37->clear();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit_2->setDate(QDate::currentDate());
    //selectedId = -1;
}

void Gcontrat::afficherContrats()
{
    Contrat::afficher(ui->tableWidget_3);
}

void Gcontrat::rechercherContratParType()
{
    QString type = ui->lineEdit_19->text().trimmed();

    if(type.isEmpty()) {
        // Si le champ est vide → afficher tous les contrats
        afficherContrats();
        ui->label_21->setText("Tous les contrats");
        return;
    }

    Contrat::rechercherParType(ui->tableWidget_3, type);
    ui->label_21->setText("Résultats pour le type : " + type);
}

void Gcontrat::trierContratsParPrix()
{
    QString choix = ui->comboBox_3->currentText();

    if (choix == "Prix croissant") {
        Contrat::trierParPrix(ui->tableWidget_3, true);
        ui->label_22->setText("Tri : Prix croissant");
    }
    else if (choix == "Prix décroissant") {
        Contrat::trierParPrix(ui->tableWidget_3, false);
        ui->label_22->setText("Tri : Prix décroissant");
    }
}



void Gcontrat::on_pushButton_10_clicked()
{
    QString type = ui->lineEdit_8->text();
    int code = ui->lineEdit_17->text().toInt();
    double prix = ui->lineEdit_18->text().toDouble();
    int id_sponsor = ui->lineEdit_35->text().toInt();
    int id_createur = ui->lineEdit_37->text().toInt();
    QDate date_signature = ui->dateEdit->date();
    QDate echeance = ui->dateEdit_2->date();

    if(type.isEmpty() || code==0 || prix<=0 || id_sponsor==0 || id_createur==0) {
        QMessageBox::warning(this,"Erreur","Champs invalides");
        return;
    }
    if(!sponsorExists(id_sponsor) || !createurExists(id_createur)) {
        QMessageBox::critical(this,"Erreur","ID Sponsor ou Créateur inexistant");
        return;
    }

    Contrat c(code, type, code, prix, date_signature, echeance, id_sponsor, id_createur);
    if(c.ajouter()) {
        QMessageBox::information(this,"Succès","Contrat ajouté !");
        clearForm();
        afficherContrats();
    } else {
        QMessageBox::critical(this,"Erreur","Échec ajout contrat");
    }
}

void Gcontrat::on_pushButton_11_clicked()
{
    if(selectedId < 0) {
        QMessageBox::warning(this,"Erreur","Sélectionne une ligne à modifier !");
        return;
    }

    QString type = ui->lineEdit_8->text();
    int code = ui->lineEdit_17->text().toInt();
    double prix = ui->lineEdit_18->text().toDouble();
    int id_sponsor = ui->lineEdit_35->text().toInt();
    int id_createur = ui->lineEdit_37->text().toInt();
    QDate date_signature = ui->dateEdit->date();
    QDate echeance = ui->dateEdit_2->date();

    Contrat c(selectedId, type, code, prix, date_signature, echeance, id_sponsor, id_createur);

    if(c.modifier()) {
        QMessageBox::information(this,"Succès","Contrat modifié !");
        clearForm();
        afficherContrats();
    } else {
        QMessageBox::critical(this,"Erreur","Échec modification contrat");
    }
}

void Gcontrat::on_pushButton_7_clicked()
{
    if(selectedId < 0) {
        QMessageBox::warning(this,"Erreur","Sélectionne une ligne à supprimer !");
        return;
    }

    Contrat c;
    if(c.supprimer(selectedId)) {
        QMessageBox::information(this,"Succès","Contrat supprimé !");
        clearForm();
        afficherContrats();
    } else {
        QMessageBox::critical(this,"Erreur","Échec suppression contrat");
    }
}

void Gcontrat::on_pushButton_14_clicked()
{
    clearForm();
}


void Gcontrat::on_tableWidget_3_cellClicked(int row)
{
    // Récupère la valeur de la première colonne (ID_CONTRAT)
    QString codeContrat = ui->tableWidget_3->item(row, 0)->text();
    selectedId = codeContrat.toInt();

    // Affecte le code contrat (même que l'ID)
    ui->lineEdit_17->setText(codeContrat); // CODE_CONTRAT = ID_CONTRAT

    // Récupère et affiche les autres champs correctement
    ui->lineEdit_8->setText(ui->tableWidget_3->item(row, 3)->text());   // TYPE
    ui->lineEdit_18->setText(ui->tableWidget_3->item(row, 4)->text());  // PRIX
    ui->lineEdit_35->setText(ui->tableWidget_3->item(row, 1)->text());  // ID_SPONSOR
    ui->lineEdit_37->setText(ui->tableWidget_3->item(row, 2)->text());  // ID_CREATEUR
    ui->dateEdit->setDate(QDate::fromString(ui->tableWidget_3->item(row, 5)->text(), "yyyy-MM-dd"));
    ui->dateEdit_2->setDate(QDate::fromString(ui->tableWidget_3->item(row, 6)->text(), "yyyy-MM-dd"));
}

void Gcontrat::on_pushButton_1_clicked()
{
    QString type = ui->lineEdit_19->text();

    if(type.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un type de contrat à rechercher !");
        return;
    }

    Contrat::rechercherParType(ui->tableWidget_3, type);

    ui->label_21->setText("Résultats pour le type : " + type);
}

void Gcontrat::on_comboBox_3_currentIndexChanged(const QString &text)
{
    bool croissant = true;

    if (text == "Prix croissant") {
        croissant = true;
        ui->label_22->setText("Tri : prix croissant");
    }
    else if (text == "Prix décroissant") {
        croissant = false;
        ui->label_22->setText("Tri : prix décroissant");
    }

    Contrat::trierParPrix(ui->tableWidget_3, croissant);
}


void Gcontrat::exporterPDF()
{
    // Vérifier si une ligne est sélectionnée
    int row = ui->tableWidget_3->currentRow();
    if(row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un contrat dans le tableau !");
        return;
    }

    // Récupérer les données du contrat
    int id_contrat = ui->tableWidget_3->item(row, 0)->text().toInt();
    int id_sponsor = ui->tableWidget_3->item(row, 1)->text().toInt();
    int id_createur = ui->tableWidget_3->item(row, 2)->text().toInt();
    QString type = ui->tableWidget_3->item(row, 3)->text();
    double prix = ui->tableWidget_3->item(row, 4)->text().toDouble();
    QString date_signature = ui->tableWidget_3->item(row, 5)->text();
    QString echeance = ui->tableWidget_3->item(row, 6)->text();

    // Récupérer les informations du créateur
    QSqlQuery queryCreateur;
    queryCreateur.prepare("SELECT NOM, PRENOM, TO_CHAR(DATE_ADHESION,'YYYY-MM-DD'), "
                          "CATEGORIE, ABONNES FROM SMART_CONTENT.CREATEUR WHERE ID_CREATEUR = :id");
    queryCreateur.bindValue(":id", id_createur);

    QString createur_nom, createur_prenom, createur_date_adhesion, createur_categorie;
    int createur_abonnes = 0;

    if(queryCreateur.exec() && queryCreateur.next()) {
        createur_nom = queryCreateur.value(0).toString();
        createur_prenom = queryCreateur.value(1).toString();
        createur_date_adhesion = queryCreateur.value(2).toString();
        createur_categorie = queryCreateur.value(3).toString();
        createur_abonnes = queryCreateur.value(4).toInt();
    }

    // Récupérer les informations du sponsor
    QSqlQuery querySponsor;
    querySponsor.prepare("SELECT NOM_SPONSOR, SECTEUR_ACTIVITE, MONTANT "
                         "FROM SMART_CONTENT.SPONSOR WHERE ID_SPONSOR = :id");
    querySponsor.bindValue(":id", id_sponsor);

    QString sponsor_nom, sponsor_secteur;
    double sponsor_montant = 0;

    if(querySponsor.exec() && querySponsor.next()) {
        sponsor_nom = querySponsor.value(0).toString();
        sponsor_secteur = querySponsor.value(1).toString();
        sponsor_montant = querySponsor.value(2).toDouble();
    }

    // Choisir l'emplacement du PDF
    QString filename = QFileDialog::getSaveFileName(this, "Exporter le contrat en PDF",
                                                    QString("Contrat_%1.pdf").arg(id_contrat),
                                                    "*.pdf");
    if(filename.isEmpty()) return;

    if(!filename.endsWith(".pdf"))
        filename += ".pdf";

    // Créer le PDF
    QPdfWriter pdf(filename);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setPageOrientation(QPageLayout::Portrait);
    pdf.setResolution(300);

    QPainter painter(&pdf);
    QRect pageRect = pdf.pageLayout().paintRectPixels(pdf.resolution());

    int margin = 400;
    int x = margin;
    int y = 200;
    int lineHeight = 100;
    int sectionSpacing = 120;

    // ========== TITRE PRINCIPAL ==========
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.setPen(Qt::black);
    QString titre = "Contrat de partenariat pour la création de contenu";
    painter.drawText(pageRect, Qt::AlignTop | Qt::AlignHCenter, titre);
    y += 220;

    // ========== DATE ==========
    painter.setFont(QFont("Arial", 10));
    QLocale frenchLocale(QLocale::French, QLocale::France);
    QString dateStr = "Le " + frenchLocale.toString(QDate::currentDate(), "dddd d MMMM yyyy");
    painter.drawText(x, y, dateStr);
    y += sectionSpacing + 20;

    // ========== ENTRE LES SOUSSIGNÉS ==========
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(x, y, "ENTRE LES SOUSSIGNÉS :");
    y += lineHeight + 30;

    // ========== SPONSOR ==========
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(x, y, sponsor_nom);
    y += lineHeight;

    painter.setFont(QFont("Arial", 9));
    painter.drawText(x + 80, y, QString("Représenté par l'ID Sponsor : %1").arg(id_sponsor));
    y += lineHeight - 50;
    painter.drawText(x + 80, y, QString("- Secteur d'activité : %1").arg(sponsor_secteur));
    y += lineHeight - 50;
    painter.drawText(x + 80, y, QString("- Montant disponible : %1 €").arg(sponsor_montant, 0, 'f', 2));
    y += lineHeight;

    QFont italicFont("Arial", 8);
    italicFont.setItalic(true);
    painter.setFont(italicFont);
    painter.drawText(x, y, QString("(ci-après désigné « le Sponsor »)"));
    y += sectionSpacing;

    // ========== ET ==========
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    QRect etRect(pageRect.width() / 2 - 200, y, 400, lineHeight);
    painter.drawText(etRect, Qt::AlignCenter, "et");
    y += sectionSpacing;

    // ========== CRÉATEUR ==========
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(x, y, QString("%1 %2").arg(createur_prenom, createur_nom));
    y += lineHeight;

    painter.setFont(QFont("Arial", 9));
    painter.drawText(x + 80, y, QString("Représenté par l'ID Créateur : %1").arg(id_createur));
    y += lineHeight - 50;
    painter.drawText(x + 80, y, QString("- Catégorie : %1").arg(createur_categorie));
    y += lineHeight - 50;
    painter.drawText(x + 80, y, QString("- Nombre d'abonnés : %1").arg(createur_abonnes));
    y += lineHeight - 50;
    painter.drawText(x + 80, y, QString("- Date d'adhésion : %1").arg(createur_date_adhesion));
    y += lineHeight;

    italicFont.setItalic(true);
    painter.setFont(italicFont);
    painter.drawText(x, y, QString("(ci-après désigné « le Créateur »)"));
    y += sectionSpacing;

    painter.setFont(QFont("Arial", 9));
    painter.drawText(x, y, "Les deux parties sont ci-après dénommées collectivement les « Parties ».");
    y += sectionSpacing + 50;

    // ========== OBJET DU CONTRAT ==========
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(x, y, "1. Objet du contrat");
    y += lineHeight;

    painter.setFont(QFont("Arial", 9));
    QString objetTexte = QString(
                             "Le présent contrat de type \"%1\" a pour objet de définir les conditions dans lesquelles "
                             "le Créateur réalisera des contenus destinés à la promotion des services du Sponsor. "
                             "Les contenus seront diffusés sur les réseaux sociaux, dans le but de valoriser "
                             "l'image et les offres du Sponsor."
                             ).arg(type);

    QRect textRect(x, y, pageRect.width() - 2 * margin, 800);
    painter.drawText(textRect, Qt::TextWordWrap, objetTexte);
    y += 300;

    // ========== DÉTAILS FINANCIERS ==========
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(x, y, "2. Modalités financières");
    y += lineHeight;

    painter.setFont(QFont("Arial", 9));
    painter.drawText(x, y, QString("- Prix du contrat : %1 €").arg(prix, 0, 'f', 2));
    y += lineHeight - 20;
    painter.drawText(x, y, QString("- Date de signature : %1").arg(date_signature));
    y += lineHeight - 20;
    painter.drawText(x, y, QString("- Date d'échéance : %1").arg(echeance));
    y += lineHeight - 20;
    painter.drawText(x, y, QString("- Code contrat : %1").arg(id_contrat));
    y += sectionSpacing + 80;

    // ========== SIGNATURES ==========
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(x, y, "Signature,");
    y += lineHeight - 20;
    painter.drawText(x, y, QString("Le %1").arg(QDate::fromString(date_signature, "yyyy-MM-dd").toString("dd/MM/yyyy")));
    y += sectionSpacing;

    int colWidth = (pageRect.width() - 2 * margin) / 2;

    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(x, y, "Le Sponsor");
    painter.drawText(x + colWidth, y, "Le Créateur");
    y += lineHeight - 20;

    painter.setFont(QFont("Arial", 9));
    painter.drawText(x, y, sponsor_nom);
    painter.drawText(x + colWidth, y, QString("%1 %2").arg(createur_prenom, createur_nom));

    painter.end();

    QMessageBox::information(this, "Succès",
                             QString("Le contrat n°%1 a été exporté avec succès !\n\nFichier : %2")
                                 .arg(id_contrat).arg(filename));
}

void Gcontrat::on_tabWidget_3_currentChanged(int index)
{
    if (index == 1)  // Onglet Statistiques
    {
        // 1. Charger les prix depuis la base
        QSqlQuery query("SELECT prix FROM contrat");
        QList<double> prices;
        while (query.next()) {
            prices.append(query.value(0).toDouble());
        }

        if (prices.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Aucun contrat trouvé !");
            return;
        }

        // 2. Définir les intervalles
        struct Interval {
            double min;
            double max;
            QColor color;
            QString label;
            int count;
        };

        QList<Interval> intervals = {
            {0, 500, Qt::blue, "0-500", 0},
            {500, 1000, Qt::blue, "500-1000", 0},
            {1000, 1500, Qt::blue, "1000-1500", 0},
            {1500, 2000, Qt::blue, "1500-2000", 0}
        };

        // 3. Compter les contrats dans chaque intervalle
        for (double p : prices) {
            for (Interval &interval : intervals) {
                if (p >= interval.min && p < interval.max) {
                    interval.count++;
                    break;
                }
            }
        }

        // 4. Couleurs modernes
        QList<QColor> colors = {
            QColor("#3a86ff"),
            QColor("#8338ec"),
            QColor("#ff2d95"),
            QColor("#ff6f91")
        };

        for (int i = 0; i < intervals.size(); ++i)
            intervals[i].color = colors[i % colors.size()];

        // 5. PieChart avec pourcentage à l'extérieur
        QPieSeries *series = new QPieSeries();
        int total = prices.size();

        for (const Interval &interval : intervals) {
            if (interval.count > 0) {

                double percentage = double(interval.count) / total * 100.0;

                // Texte extérieur (seulement le %)
                QString sliceLabel = QString("%1%")
                                         .arg(QString::number(percentage, 'f', 1));

                QPieSlice *slice = series->append(interval.label, interval.count);

                slice->setBrush(interval.color);
                slice->setLabel(sliceLabel);

                // 🔥 Afficher le texte à l'extérieur du graphe
                slice->setLabelVisible(true);
                slice->setLabelPosition(QPieSlice::LabelOutside);

                slice->setLabelColor(Qt::black); // couleur du texte
                slice->setLabelArmLengthFactor(0.15); // longueur du trait
            }
        }

        // 6. Construire le graphique
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Répartition des contrats par intervalle de prix");

        // Légende à droite (avec les intervalles)
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignRight);
        chart->legend()->setMarkerShape(QLegend::MarkerShapeCircle);

        QChartView chartView(chart);
        chartView.resize(650, 450);
        chartView.setRenderHint(QPainter::Antialiasing);

        // Exporter en image
        QPixmap pixmap = chartView.grab();
        pixmap.save("stat_prix.png");

        ui->label_stat->setPixmap(
            pixmap.scaled(ui->label_stat->size(),
                          Qt::KeepAspectRatio,
                          Qt::SmoothTransformation)
            );
    }
}


// intervalles
/*void Gcontrat::on_tabWidget_3_currentChanged(int index)
{
    // Si on ouvre l'onglet Statistiques (index = 1)
    if (index == 1)
    {
        // 1. Charger les prix depuis la base
        QSqlQuery query("SELECT prix FROM contrat");
        QList<double> prices;
        while (query.next()) {
            prices.append(query.value(0).toDouble());
        }

        if (prices.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Aucun contrat trouvé !");
            return;
        }

        // 2. Définir les intervalles
        struct Interval {
            double min;
            double max;
            QColor color;
            QString label;
            int count;
        };

        QList<Interval> intervals = {
            {0, 500, Qt::blue, "[0-500]", 0},
            {500, 1000, Qt::blue, "[500-1000]", 0},
            {1000, 1500, Qt::blue, "[1000-1500]", 0},
            {1500, 2000, Qt::blue, "[1500-2000]", 0}
        };

        // 3. Compter le nombre de contrats dans chaque intervalle
        for(double p : prices) {
            for(Interval &interval : intervals) {
                if(p >= interval.min && p < interval.max) {
                    interval.count++;
                    break;
                }
            }
        }

        // 4. Appliquer des couleurs harmonisées avec l'interface
        QList<QColor> colors = {
            QColor("#3a86ff"), // Bleu
            QColor("#8338ec"), // Violet
            QColor("#ff2d95"), // Rose vif
            QColor("#ff6f91")  // Rose clair
        };

        for(int i = 0; i < intervals.size(); ++i) {
            intervals[i].color = colors[i % colors.size()];
        }

        // 5. Créer la série pour le PieChart
        QPieSeries *series = new QPieSeries();
        for(const Interval &interval : intervals) {
            if(interval.count > 0) {
                QPieSlice *slice = series->append(interval.label + QString(" (%1)").arg(interval.count), interval.count);
                slice->setBrush(interval.color);
            }
        }

        // 6. Construire et afficher le graphique
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Répartition des contrats par intervalle de prix");

        QChartView chartView(chart);
        chartView.resize(600, 400);

        QPixmap pixmap = chartView.grab();
        pixmap.save("stat_prix.png");

        ui->label_stat->setPixmap(
            pixmap.scaled(ui->label_stat->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    }
}*/


void Gcontrat::updateAlertIcon(bool hasAlert)
{
    if (hasAlert) {
        ui->label_alert->setPixmap(QPixmap(":/images/alert.png").scaled(62, 62, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // Icône grise (optionnelle) sinon on efface
        ui->label_alert->setPixmap(QPixmap());
    }
}


void Gcontrat::showAlertContracts()
{
    QSqlQuery query;
    query.prepare("SELECT ID_CONTRAT, TYPE, ECHEANCE "
                  "FROM SMART_CONTENT.CONTRAT "
                  "WHERE ECHEANCE <= SYSDATE + 7 AND ECHEANCE >= SYSDATE "
                  "ORDER BY ECHEANCE ASC");

    if (!query.exec()) {
        qDebug() << "Erreur SQL:" << query.lastError().text();
        return;
    }

    QString msg = "📋 Liste des contrats à échéance proche :\n\n";
    int count = 0;

    while (query.next()) {
        count++;
        int id = query.value("ID_CONTRAT").toInt();
        QString type = query.value("TYPE").toString();
        QDate echeance = query.value("ECHEANCE").toDate();
        int joursRestants = QDate::currentDate().daysTo(echeance);

        msg += QString("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        msg += QString("Contrat #%1\n").arg(id);
        msg += QString("  Type       : %1\n").arg(type);
        msg += QString("  Échéance   : %1\n").arg(echeance.toString("dd/MM/yyyy"));
        msg += QString("  Jours rest.: %1 jour(s)\n\n").arg(joursRestants);
    }

    if (count == 0) {
        QMessageBox::information(this, "Alertes Contrats",
                                 "✅ Aucun contrat proche de l'échéance.");
        return;
    }

    msg += QString("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    msg += QString("\nTotal : %1 contrat(s) nécessitant votre attention").arg(count);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("⚠ Alertes Contrats - Échéances Proches");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(QString("Vous avez %1 contrat(s) arrivant à échéance dans les 7 prochains jours.").arg(count));
    msgBox.setDetailedText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);

    msgBox.setStyleSheet(
        "QMessageBox { "
        "   background-color: white; "
        "   min-width: 700px; "
        "   min-height: 400px; "
        "} "
        "QLabel { "
        "   color: black; "
        "   font-size: 12pt; "
        "   background-color: white; "
        "} "
        "QTextEdit { "
        "   background-color: #f8f9fa; "
        "   color: black; "
        "   font-size: 10pt; "
        "   font-family: 'Courier New', monospace; "
        "   border: 1px solid #dee2e6; "
        "   padding: 10px; "
        "} "
        "QPushButton { "
        "   background-color: #FF8C00; "
        "   color: white; "
        "   border-radius: 5px; "
        "   padding: 8px 20px; "
        "   font-weight: bold; "
        "   min-width: 80px; "
        "} "
        "QPushButton:hover { "
        "   background-color: #FFA500; "
        "}"
        );

    msgBox.exec();
}


void Gcontrat::checkContractAlerts()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*), MIN(ECHEANCE) "
                  "FROM SMART_CONTENT.CONTRAT "
                  "WHERE ECHEANCE <= SYSDATE + 7 AND ECHEANCE >= SYSDATE");

    if (!query.exec()) {
        qDebug() << "Erreur vérification alertes:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        int count = query.value(0).toInt();

        if (count > 0) {
            QDate prochaine_echeance = query.value(1).toDate();
            int joursRestants = QDate::currentDate().daysTo(prochaine_echeance);

            QString titre = "⚠ Alerte Contrats";
            QString message;

            if (count == 1) {
                message = QString("1 contrat arrive à échéance dans %1 jour(s) !\n"
                                  "Date d'échéance : %2\n\n"
                                  "Cliquez pour voir les détails.")
                              .arg(joursRestants)
                              .arg(prochaine_echeance.toString("dd/MM/yyyy"));
            } else {
                message = QString("%1 contrats arrivent à échéance prochainement !\n"
                                  "Prochaine échéance dans %2 jour(s)\n\n"
                                  "Cliquez pour voir les détails.")
                              .arg(count)
                              .arg(joursRestants);
            }

            // Afficher la notification système
            if (trayIcon && trayIcon->isVisible()) {
                trayIcon->showMessage(titre, message,
                                      QSystemTrayIcon::Warning,
                                      10000); // Afficher pendant 10 secondes
            }

            // Mettre à jour l'icône d'alerte dans l'interface
            updateAlertIcon(true);
        } else {
            updateAlertIcon(false);
        }
    }
}


bool Gcontrat::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->label_alert && event->type() == QEvent::MouseButtonPress) {
        showAlertContracts();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}


// ==================== FONCTION createFloatingChatbot() ====================

void Gcontrat::createFloatingChatbot()
{
    // ===== BOUTON FLOTTANT =====
    chatbotFloatingButton = new QPushButton(this);
    chatbotFloatingButton->setText("💬");
    chatbotFloatingButton->setFixedSize(60, 60);
    chatbotFloatingButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF8C00;"
        "   color: white;"
        "   border-radius: 30px;"
        "   font-size: 24px;"
        "   border: 3px solid white;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FFA500;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #FF7F00;"
        "}"
        );

    // Position en bas à droite (plus haut)
    chatbotFloatingButton->move(this->width() - 80, this->height() - 150);
    chatbotFloatingButton->raise();
    chatbotFloatingButton->setCursor(Qt::PointingHandCursor);

    connect(chatbotFloatingButton, &QPushButton::clicked, this, &Gcontrat::toggleChatbot);

    // ===== FENÊTRE CHATBOT =====
    chatbotWindow = new QWidget(this);
    chatbotWindow->setFixedSize(450, 600);
    chatbotWindow->setStyleSheet(
        "QWidget#chatbotWindow {"
        "   background-color: white;"
        "   border-radius: 15px;"
        "   border: 2px solid #ccc;"
        "}"
        );
    chatbotWindow->setObjectName("chatbotWindow");
    chatbotWindow->hide();

    // Layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(chatbotWindow);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ===== EN-TÊTE =====
    QWidget *header = new QWidget();
    header->setFixedHeight(60);
    header->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #667eea, stop:1 #764ba2);"
        "border-top-left-radius: 15px;"
        "border-top-right-radius: 15px;"
        );

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(15, 0, 15, 0);

    QLabel *titleLabel = new QLabel("💬 Assistant Contrats");
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 16px;"
        "font-weight: bold;"
        );

    QPushButton *closeBtn = new QPushButton("✕");
    closeBtn->setFixedSize(30, 30);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   font-size: 20px;"
        "   border: none;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255,255,255,0.2);"
        "   border-radius: 15px;"
        "}"
        );
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &Gcontrat::toggleChatbot);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(closeBtn);

    mainLayout->addWidget(header);

    // ===== ZONE DE CHAT =====
    textEdit_chatbot = new QTextEdit();
    textEdit_chatbot->setReadOnly(true);
    textEdit_chatbot->setStyleSheet(
        "QTextEdit {"
        "   background-color: #f5f5f5;"
        "   border: none;"
        "   padding: 15px;"
        "   font-size: 13px;"
        "   font-family: 'Segoe UI', Arial;"
        "}"
        );
    mainLayout->addWidget(textEdit_chatbot);

    // ===== SUGGESTIONS RAPIDES =====
    QWidget *suggestionsWidget = new QWidget();
    suggestionsWidget->setStyleSheet("background-color: white; padding: 5px;");
    QHBoxLayout *suggestionsLayout = new QHBoxLayout(suggestionsWidget);
    suggestionsLayout->setSpacing(5);
    suggestionsLayout->setContentsMargins(10, 5, 10, 5);

    QStringList suggestions = {"📊 Combien?", "⏰ Expirés?", "💰 Prix moyen?", "❓ Aide"};
    for (const QString &text : suggestions) {
        QPushButton *btn = new QPushButton(text);
        btn->setStyleSheet(
            "QPushButton {"
            "   background-color: #E3F2FD;"
            "   color: #1976D2;"
            "   border: 1px solid #90CAF9;"
            "   border-radius: 12px;"
            "   padding: 5px 10px;"
            "   font-size: 11px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #BBDEFB;"
            "}"
            );
        btn->setCursor(Qt::PointingHandCursor);

        connect(btn, &QPushButton::clicked, [this, text]() {
            QString query = text;
            query.remove(QRegularExpression("[📊⏰💰❓]")).trimmed();
            lineEdit_chatbot->setText(query);
            on_pushButton_chatbot_clicked();
        });

        suggestionsLayout->addWidget(btn);
    }

    mainLayout->addWidget(suggestionsWidget);

    // ===== ZONE DE SAISIE =====
    QWidget *inputWidget = new QWidget();
    inputWidget->setStyleSheet(
        "background-color: white;"
        "border-bottom-left-radius: 15px;"
        "border-bottom-right-radius: 15px;"
        );
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setContentsMargins(10, 10, 10, 10);
    inputLayout->setSpacing(8);

    lineEdit_chatbot = new QLineEdit();
    lineEdit_chatbot->setPlaceholderText("Posez votre question...");
    lineEdit_chatbot->setStyleSheet(
        "QLineEdit {"
        "   background-color: #f5f5f5;"
        "   border: 1px solid #ddd;"
        "   border-radius: 20px;"
        "   padding: 10px 15px;"
        "   font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #667eea;"
        "   background-color: white;"
        "}"
        );

    pushButton_chatbot = new QPushButton("➤");
    pushButton_chatbot->setFixedSize(40, 40);
    pushButton_chatbot->setStyleSheet(
        "QPushButton {"
        "   background-color: #667eea;"
        "   color: white;"
        "   border-radius: 20px;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #764ba2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #5568d3;"
        "}"
        );
    pushButton_chatbot->setCursor(Qt::PointingHandCursor);

    connect(lineEdit_chatbot, &QLineEdit::returnPressed, this, &Gcontrat::on_pushButton_chatbot_clicked);
    connect(pushButton_chatbot, &QPushButton::clicked, this, &Gcontrat::on_pushButton_chatbot_clicked);

    inputLayout->addWidget(lineEdit_chatbot);
    inputLayout->addWidget(pushButton_chatbot);

    mainLayout->addWidget(inputWidget);

    // Message de bienvenue
    addChatMessage("Assistant", "👋 Bonjour ! Je suis votre assistant pour les contrats.\n\n"
                                "Utilisez les boutons de suggestion ou posez-moi directement vos questions !", true);
}


// ==================== FONCTION toggleChatbot() ====================

void Gcontrat::toggleChatbot()
{
    chatbotVisible = !chatbotVisible;

    if (chatbotVisible) {
        // Positionner la fenêtre en bas à droite (plus haut)
        int x = this->width() - chatbotWindow->width() - 90;
        int y = this->height() - chatbotWindow->height() - 100;
        chatbotWindow->move(x, y);
        chatbotWindow->show();
        chatbotWindow->raise();
        lineEdit_chatbot->setFocus();
    } else {
        chatbotWindow->hide();
    }
}


// ==================== FONCTION resizeEvent() ====================

void Gcontrat::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // Repositionner le bouton flottant (plus haut)
    if (chatbotFloatingButton) {
        chatbotFloatingButton->move(this->width() - 80, this->height() - 150);
    }

    // Repositionner la fenêtre chatbot si visible (plus haut)
    if (chatbotVisible && chatbotWindow) {
        int x = this->width() - chatbotWindow->width() - 90;
        int y = this->height() - chatbotWindow->height() - 100;
        chatbotWindow->move(x, y);
    }

    // Si intégré, faire occuper à stackedWidget tout l'espace disponible
    if (m_embedded && ui->stackedWidget && ui->centralwidget) {
        ui->stackedWidget->setGeometry(ui->centralwidget->rect());
    }
}


// ==================== FONCTION on_pushButton_chatbot_clicked() ====================

void Gcontrat::on_pushButton_chatbot_clicked()
{
    QString userMessage = lineEdit_chatbot->text().trimmed();

    if (userMessage.isEmpty()) {
        return;
    }

    // Afficher le message de l'utilisateur
    addChatMessage("Vous", userMessage, false);

    // Vider le champ de saisie
    lineEdit_chatbot->clear();

    // Traiter le message
    processChatbotMessage(userMessage);
}


// ==================== FONCTION addChatMessage() ====================

void Gcontrat::addChatMessage(const QString& sender, const QString& message, bool isBot)
{
    QString color = isBot ? "#4CAF50" : "#2196F3";
    QString alignment = isBot ? "left" : "right";
    QString bgColor = isBot ? "#E8F5E9" : "#E3F2FD";

    QString html = QString(
                       "<div style='margin: 10px; text-align: %1;'>"
                       "  <div style='display: inline-block; max-width: 70%%; background-color: %2; "
                       "              border-radius: 10px; padding: 10px; text-align: left;'>"
                       "    <b style='color: %3;'>%4:</b><br>"
                       "    <span style='color: #333; white-space: pre-wrap;'>%5</span>"
                       "  </div>"
                       "</div>"
                       ).arg(alignment, bgColor, color, sender, message.toHtmlEscaped().replace("\n", "<br>"));

    textEdit_chatbot->append(html);

    // Scroller vers le bas
    QTextCursor cursor = textEdit_chatbot->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit_chatbot->setTextCursor(cursor);
}


// ==================== FONCTION processChatbotMessage() ====================
// [Copiez ici la fonction processChatbotMessage() du code précédent]
// Elle contient toute la logique de traitement des questions

void Gcontrat::processChatbotMessage(const QString& message)
{
    QString lowerMessage = message.toLower();
    QString response;

    // ========== AIDE ==========
    if (lowerMessage.contains("aide") || lowerMessage.contains("help") || lowerMessage == "?") {
        response = "📚 Commandes disponibles :\n\n"
                   "📊 Statistiques :\n"
                   "  • Combien de contrats ?\n"
                   "  • Prix total ?\n"
                   "  • Prix moyen ?\n"
                   "  • Contrat le plus cher ?\n\n"
                   "⏰ Échéances :\n"
                   "  • Contrats expirés ?\n"
                   "  • Contrats à expirer ?\n"
                   "  • Échéances cette semaine ?\n\n"
                   "🔍 Recherche :\n"
                   "  • Contrats de [type] ?\n"
                   "  • Contrats avec sponsor [nom] ?\n\n"
                   "💰 Finances :\n"
                   "  • Top sponsors ?\n\n"
                   "⚖️ Questions Juridiques :\n"
                   "  • Qu'est-ce qu'un contrat ?\n"
                   "  • Durée d'un contrat ?\n"
                   "  • Résiliation ?\n"
                   "  • Clause de confidentialité ?\n"
                   "  • Force majeure ?\n\n"
                   "💼 Questions Financières :\n"
                   "  • Modalités de paiement ?\n"
                   "  • TVA ?\n"
                   "  • Pénalités de retard ?\n"
                   "  • Budget disponible ?";
    }

    // ========== NOMBRE DE CONTRATS ==========
    else if (lowerMessage.contains("combien") || lowerMessage.contains("nombre")) {
        QSqlQuery query("SELECT COUNT(*) FROM SMART_CONTENT.CONTRAT");
        if (query.next()) {
            int count = query.value(0).toInt();
            response = QString("📊 Vous avez actuellement %1 contrat(s) dans la base de données.").arg(count);
        }
    }

    // ========== PRIX TOTAL ==========
    else if (lowerMessage.contains("prix total") || lowerMessage.contains("montant total")) {
        QSqlQuery query("SELECT SUM(PRIX) FROM SMART_CONTENT.CONTRAT");
        if (query.next()) {
            double total = query.value(0).toDouble();
            response = QString("💰 Le montant total de tous les contrats est de %1 DT").arg(total, 0, 'f', 2);
        }
    }

    // ========== PRIX MOYEN ==========
    else if (lowerMessage.contains("prix moyen") || lowerMessage.contains("moyenne")) {
        QSqlQuery query("SELECT AVG(PRIX) FROM SMART_CONTENT.CONTRAT");
        if (query.next()) {
            double moyenne = query.value(0).toDouble();
            response = QString("📈 Le prix moyen des contrats est de %1 DT").arg(moyenne, 0, 'f', 2);
        }
    }

    // ========== CONTRAT LE PLUS CHER ==========
    else if (lowerMessage.contains("plus cher") || lowerMessage.contains("maximum")) {
        QSqlQuery query("SELECT ID_CONTRAT, TYPE, PRIX FROM SMART_CONTENT.CONTRAT ORDER BY PRIX DESC");
        if (query.next()) {
            int id = query.value(0).toInt();
            QString type = query.value(1).toString();
            double prix = query.value(2).toDouble();
            response = QString("💎 Le contrat le plus cher est :\n"
                               "  • ID: %1\n"
                               "  • Type: %2\n"
                               "  • Prix: %3 DT").arg(id).arg(type).arg(prix, 0, 'f', 2);
        }
    }

    // ========== CONTRATS EXPIRÉS ==========
    else if (lowerMessage.contains("expir") || lowerMessage.contains("périm")) {
        QSqlQuery query("SELECT COUNT(*) FROM SMART_CONTENT.CONTRAT WHERE ECHEANCE < SYSDATE");
        if (query.next()) {
            int count = query.value(0).toInt();
            if (count == 0) {
                response = "✅ Aucun contrat expiré !";
            } else {
                response = QString("⚠ Attention ! Vous avez %1 contrat(s) expiré(s).\n"
                                   "Consultez la liste pour plus de détails.").arg(count);
            }
        }
    }

    // ========== CONTRATS À EXPIRER ==========
    else if (lowerMessage.contains("expirer") || lowerMessage.contains("bientôt") ||
             lowerMessage.contains("proche")) {
        QSqlQuery query("SELECT COUNT(*) FROM SMART_CONTENT.CONTRAT "
                        "WHERE ECHEANCE BETWEEN SYSDATE AND SYSDATE + 7");
        if (query.next()) {
            int count = query.value(0).toInt();
            if (count == 0) {
                response = "✅ Aucun contrat n'expire dans les 7 prochains jours.";
            } else {
                response = QString("⏰ %1 contrat(s) arrivent à échéance dans les 7 prochains jours !\n"
                                   "Cliquez sur l'icône d'alerte pour voir les détails.").arg(count);
            }
        }
    }

    // ========== TOP SPONSORS ==========
    else if (lowerMessage.contains("top sponsor") || lowerMessage.contains("meilleur sponsor")) {
        QSqlQuery query(
            "SELECT S.NOM_SPONSOR, COUNT(*) as NB, SUM(C.PRIX) as TOTAL "
            "FROM SMART_CONTENT.CONTRAT C "
            "JOIN SMART_CONTENT.SPONSOR S ON C.ID_SPONSOR = S.ID_SPONSOR "
            "GROUP BY S.NOM_SPONSOR "
            "ORDER BY TOTAL DESC"
            );

        response = "🏆 Top Sponsors :\n\n";
        int rank = 1;
        while (query.next() && rank <= 5) {
            QString nom = query.value(0).toString();
            int nb = query.value(1).toInt();
            double total = query.value(2).toDouble();
            response += QString("%1. %2\n   • %3 contrat(s) - %4 €\n\n")
                            .arg(rank).arg(nom).arg(nb).arg(total, 0, 'f', 2);
            rank++;
        }

        if (rank == 1) {
            response = "❌ Aucun sponsor trouvé dans la base de données.";
        }
    }

    // ========== ÉCHÉANCES CETTE SEMAINE ==========
    else if (lowerMessage.contains("semaine") || lowerMessage.contains("7 jours")) {
        QSqlQuery query(
            "SELECT ID_CONTRAT, TYPE, ECHEANCE FROM SMART_CONTENT.CONTRAT "
            "WHERE ECHEANCE BETWEEN SYSDATE AND SYSDATE + 7 "
            "ORDER BY ECHEANCE ASC"
            );

        int count = 0;
        response = "📅 Échéances des 7 prochains jours :\n\n";

        while (query.next()) {
            count++;
            int id = query.value(0).toInt();
            QString type = query.value(1).toString();
            QDate echeance = query.value(2).toDate();
            int jours = QDate::currentDate().daysTo(echeance);

            response += QString("• Contrat #%1 (%2)\n  Échéance: %3 (dans %4 jour(s))\n\n")
                            .arg(id).arg(type).arg(echeance.toString("dd/MM/yyyy")).arg(jours);
        }

        if (count == 0) {
            response = "✅ Aucune échéance dans les 7 prochains jours !";
        }
    }

    // ========== QU'EST-CE QU'UN CONTRAT ? ==========
    else if (lowerMessage.contains("qu'est-ce qu'un contrat") ||
             lowerMessage.contains("définition contrat") ||
             lowerMessage.contains("c'est quoi un contrat")) {
        response = "⚖️ Définition juridique d'un contrat :\n\n"
                   "Un contrat est un accord de volonté entre deux ou plusieurs parties "
                   "destiné à créer, modifier, transmettre ou éteindre des obligations.\n\n"
                   "📋 Éléments essentiels :\n"
                   "  • Consentement des parties\n"
                   "  • Capacité juridique\n"
                   "  • Objet certain et licite\n"
                   "  • Cause licite\n\n"
                   "Dans votre système, un contrat lie un sponsor et un créateur de contenu "
                   "avec des obligations mutuelles (création de contenu contre rémunération).";
    }

    // ========== DURÉE D'UN CONTRAT ==========
    else if (lowerMessage.contains("durée") ||
             lowerMessage.contains("combien de temps") ||
             lowerMessage.contains("période")) {
        QSqlQuery query(
            "SELECT AVG(ECHEANCE - DATE_SIGNATURE) as DUREE_MOYENNE "
            "FROM SMART_CONTENT.CONTRAT"
            );

        if (query.next()) {
            double duree = query.value(0).toDouble();
            response = QString("⏱️ Durée des contrats :\n\n"
                               "La durée moyenne de vos contrats est de %1 jours (environ %2 mois).\n\n"
                               "📌 Bon à savoir :\n"
                               "  • Un contrat peut être à durée déterminée (CDD) ou indéterminée (CDI)\n"
                               "  • La durée doit être clairement mentionnée\n"
                               "  • Au-delà de l'échéance, le contrat peut être renouvelé ou résilié")
                           .arg(duree, 0, 'f', 0).arg(duree / 30, 0, 'f', 1);
        }
    }

    // ========== RÉSILIATION ==========
    else if (lowerMessage.contains("résili") ||
             lowerMessage.contains("annul") ||
             lowerMessage.contains("rompre")) {
        response = "⚠️ Résiliation d'un contrat :\n\n"
                   "📋 Motifs de résiliation :\n"
                   "  • Arrivée du terme (fin normale)\n"
                   "  • Accord mutuel des parties\n"
                   "  • Inexécution des obligations\n"
                   "  • Force majeure\n"
                   "  • Clause résolutoire prévue\n\n"
                   "⚖️ Conséquences juridiques :\n"
                   "  • Mise en demeure préalable recommandée\n"
                   "  • Possibilité de dommages-intérêts\n"
                   "  • Restitution des prestations si nécessaire\n\n"
                   "💡 Conseil : Toujours documenter par écrit les raisons de résiliation.";
    }

    // ========== CLAUSE DE CONFIDENTIALITÉ ==========
    else if (lowerMessage.contains("confidentialité") ||
             lowerMessage.contains("secret") ||
             lowerMessage.contains("nda")) {
        response = "🔒 Clause de confidentialité (NDA) :\n\n"
                   "📋 Définition :\n"
                   "Une clause qui oblige les parties à ne pas divulguer les informations "
                   "sensibles échangées dans le cadre du contrat.\n\n"
                   "🎯 Éléments à inclure :\n"
                   "  • Nature des informations confidentielles\n"
                   "  • Durée de la confidentialité\n"
                   "  • Exceptions (informations publiques)\n"
                   "  • Sanctions en cas de violation\n\n"
                   "💼 Dans vos contrats :\n"
                   "Les créateurs doivent protéger les stratégies marketing des sponsors "
                   "et les sponsors doivent respecter les données personnelles des créateurs.";
    }

    // ========== FORCE MAJEURE ==========
    else if (lowerMessage.contains("force majeure") ||
             lowerMessage.contains("cas fortuit") ||
             lowerMessage.contains("événement imprévisible")) {
        response = "⚡ Force majeure :\n\n"
                   "📋 Définition juridique :\n"
                   "Événement extérieur, imprévisible et irrésistible qui empêche "
                   "l'exécution du contrat.\n\n"
                   "🌪️ Exemples :\n"
                   "  • Catastrophes naturelles (séisme, inondation)\n"
                   "  • Guerre, émeutes\n"
                   "  • Épidémie/pandémie\n"
                   "  • Interdiction légale survenue après signature\n\n"
                   "⚖️ Conséquences :\n"
                   "  • Suspension temporaire des obligations\n"
                   "  • Ou résiliation sans pénalités\n"
                   "  • Les parties doivent en informer rapidement\n\n"
                   "💡 Important : La force majeure doit être prouvée et notifiée.";
    }

    // ========== MODALITÉS DE PAIEMENT ==========
    else if (lowerMessage.contains("paiement") ||
             lowerMessage.contains("payer") ||
             lowerMessage.contains("modalités financières")) {
        response = "💳 Modalités de paiement dans un contrat :\n\n"
                   "📋 Éléments à préciser :\n"
                   "  • Montant exact ou mode de calcul\n"
                   "  • Échéances de paiement\n"
                   "  • Moyens de paiement acceptés\n"
                   "  • Délai de paiement (ex: 30 jours fin de mois)\n\n"
                   "💰 Types de paiement courants :\n"
                   "  • Paiement comptant (à la signature)\n"
                   "  • Paiement échelonné (mensuel, trimestriel)\n"
                   "  • Paiement à la livraison\n"
                   "  • Acompte + solde\n\n"
                   "⚠️ Bon à savoir :\n"
                   "Le retard de paiement peut entraîner des pénalités contractuelles.";
    }

    // ========== TVA ==========
    else if (lowerMessage.contains("tva") ||
             lowerMessage.contains("taxe") ||
             lowerMessage.contains("hors taxes") ||
             lowerMessage.contains("ttc")) {
        response = "🧾 TVA sur les contrats :\n\n"
                   "📋 Principes de base :\n"
                   "  • TVA = Taxe sur la Valeur Ajoutée\n"
                   "  • Taux en Tunisie : généralement 19%\n"
                   "  • Certains services : taux réduit (7% ou 13%)\n\n"
                   "💼 Dans vos contrats :\n"
                   "  • Prix HT (Hors Taxes) = montant de base\n"
                   "  • Prix TTC = HT + TVA\n"
                   "  • Toujours préciser si le prix est HT ou TTC\n\n"
                   "📊 Exemple :\n"
                   "  • Prix HT : 1000 €\n"
                   "  • TVA 19% : 190 €\n"
                   "  • Prix TTC : 1190 €\n\n"
                   "⚠️ Important : Les auto-entrepreneurs peuvent être exonérés de TVA.";
    }

    // ========== PÉNALITÉS DE RETARD ==========
    else if (lowerMessage.contains("pénalité") ||
             lowerMessage.contains("retard") ||
             lowerMessage.contains("sanctions financières")) {
        response = "⏰ Pénalités de retard :\n\n"
                   "📋 Définition :\n"
                   "Sommes dues en cas de retard dans l'exécution des obligations "
                   "(paiement, livraison, etc.)\n\n"
                   "💰 Types de pénalités :\n"
                   "  • Pénalités de retard de paiement\n"
                   "  • Intérêts légaux (taux fixé par la loi)\n"
                   "  • Indemnité forfaitaire de recouvrement (40€)\n"
                   "  • Clause pénale contractuelle\n\n"
                   "📊 Calcul typique :\n"
                   "  • Taux légal ou contractuel (ex: 2% par mois)\n"
                   "  • Application automatique dès le 1er jour de retard\n\n"
                   "⚖️ Cadre légal :\n"
                   "Les pénalités doivent être mentionnées dans le contrat et ne peuvent "
                   "pas être abusives (proportionnelles au préjudice).";
    }

    // ========== BUDGET DISPONIBLE ==========
    else if (lowerMessage.contains("budget") ||
             lowerMessage.contains("fonds disponible") ||
             lowerMessage.contains("enveloppe")) {
        QSqlQuery queryTotal("SELECT SUM(PRIX) FROM SMART_CONTENT.CONTRAT");
        QSqlQuery querySponsor("SELECT SUM(MONTANT) FROM SMART_CONTENT.SPONSOR");

        double totalContrats = 0;
        double totalBudgetSponsors = 0;

        if (queryTotal.next()) {
            totalContrats = queryTotal.value(0).toDouble();
        }

        if (querySponsor.next()) {
            totalBudgetSponsors = querySponsor.value(0).toDouble();
        }

        double budgetRestant = totalBudgetSponsors - totalContrats;
        double tauxUtilisation = (totalContrats / totalBudgetSponsors) * 100;

        response = QString("💼 Analyse budgétaire :\n\n"
                           "💰 Budget total des sponsors : %1 €\n"
                           "📊 Montant des contrats signés : %2 €\n"
                           "✅ Budget disponible : %3 €\n"
                           "📈 Taux d'utilisation : %4%\n\n"
                           "📋 Recommandations :\n")
                       .arg(totalBudgetSponsors, 0, 'f', 2)
                       .arg(totalContrats, 0, 'f', 2)
                       .arg(budgetRestant, 0, 'f', 2)
                       .arg(tauxUtilisation, 0, 'f', 1);

        if (tauxUtilisation > 90) {
            response += "  ⚠️ Attention : Budget presque épuisé !\n"
                        "  • Envisagez de rechercher de nouveaux sponsors\n"
                        "  • Ou renégociez les budgets existants";
        } else if (tauxUtilisation > 70) {
            response += "  ⚡ Budget bien utilisé\n"
                        "  • Continuez à surveiller les dépenses\n"
                        "  • Anticipez les futurs besoins";
        } else {
            response += "  ✅ Budget sain et disponible\n"
                        "  • Vous pouvez signer de nouveaux contrats\n"
                        "  • Opportunités de croissance disponibles";
        }
    }

    // ========== DROITS D'AUTEUR ==========
    else if (lowerMessage.contains("droits d'auteur") ||
             lowerMessage.contains("propriété intellectuelle") ||
             lowerMessage.contains("copyright")) {
        response = "©️ Droits d'auteur et propriété intellectuelle :\n\n"
                   "📋 Principes de base :\n"
                   "  • Les créations du créateur sont protégées automatiquement\n"
                   "  • Le créateur conserve ses droits moraux (paternité, respect)\n"
                   "  • Les droits patrimoniaux peuvent être cédés ou concédés\n\n"
                   "💼 Dans un contrat de création :\n"
                   "  • Préciser qui détient les droits (créateur ou sponsor)\n"
                   "  • Définir l'étendue de la cession (durée, territoire)\n"
                   "  • Prévoir la rémunération pour la cession\n\n"
                   "⚖️ Types de licences :\n"
                   "  • Licence exclusive : seul le sponsor peut utiliser\n"
                   "  • Licence non-exclusive : utilisation partagée possible\n"
                   "  • Cession totale : transfert complet des droits\n\n"
                   "💡 Important : Sans clause spécifique, le créateur garde ses droits !";
    }

    // ========== OBLIGATIONS LÉGALES ==========
    else if (lowerMessage.contains("obligations légales") ||
             lowerMessage.contains("mentions obligatoires") ||
             lowerMessage.contains("que doit contenir")) {
        response = "📜 Mentions obligatoires d'un contrat :\n\n"
                   "✅ Éléments essentiels :\n"
                   "  • Identification des parties (nom, adresse, SIRET)\n"
                   "  • Date de signature\n"
                   "  • Objet du contrat (description précise)\n"
                   "  • Prix et modalités de paiement\n"
                   "  • Durée et date d'échéance\n"
                   "  • Obligations de chaque partie\n\n"
                   "📋 Clauses recommandées :\n"
                   "  • Clause de résiliation\n"
                   "  • Clause de confidentialité\n"
                   "  • Clause de force majeure\n"
                   "  • Propriété intellectuelle\n"
                   "  • Juridiction compétente en cas de litige\n\n"
                   "⚠️ Attention : L'absence d'éléments essentiels peut rendre "
                   "le contrat invalide ou difficile à faire respecter.";
    }

    // ========== MESSAGE NON COMPRIS ==========
    else {
        response = "🤔 Désolé, je n'ai pas compris votre question.\n\n"
                   "Tapez 'aide' pour voir la liste des commandes disponibles.";
    }

    // Afficher la réponse du bot
    addChatMessage("Assistant", response, true);
}
