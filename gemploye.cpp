#include "gemploye.h"
#include "ui_gemploye.h"
#include <QPixmap>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QTableWidgetItem>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSqlQuery>
#include <QDir>
#include <QFileDialog>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextToSpeech>
#include <QTimer>
#include <QtAlgorithms>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QSettings>
#include <cmath>
#include <QTextStream>
#include <QPdfWriter>
#include <QPainter>

#include <QtCharts>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QHorizontalBarSeries>

Gemploye::Gemploye(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gemploye)
    , modeModification(false)
    , idSponsorEnCours(-1)
{
    ui->setupUi(this);

    QPixmap logo(":/images/logo.png");
    if(!logo.isNull()) {
        ui->label_7->setPixmap(logo.scaled(ui->label_7->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->label_44->setPixmap(logo.scaled(ui->label_44->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->label_45->setPixmap(logo.scaled(ui->label_45->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->label_46->setPixmap(logo.scaled(ui->label_46->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->label_47->setPixmap(logo.scaled(ui->label_47->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->label_48->setPixmap(logo.scaled(ui->label_48->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    setupSponsorsValidators();
    peuplerSecteursActivite();
    peuplerStatuts();
    chargerStatuts();

    QTimer::singleShot(500, this, [this]() {
        actualiserTableauSponsors();
    });

    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->tabWidget_4, &QTabWidget::currentChanged, this, &Gemploye::on_tabWidget_4_currentChanged);
    connect(ui->tableWidget_4, &QTableWidget::cellDoubleClicked, this, &Gemploye::on_tableWidget_4_cellDoubleClicked);

    syntheseVocale = new QTextToSpeech(this);

    QVector<QVoice> voices = syntheseVocale->availableVoices();
    for (const QVoice &voice : voices) {
        if (voice.name().contains("french", Qt::CaseInsensitive) ||
            voice.name().contains("français", Qt::CaseInsensitive)) {
            syntheseVocale->setVoice(voice);
            break;
        }
    }

    syntheseVocale->setRate(0.0);
}

Gemploye::~Gemploye()
{
    sauvegarderStatuts();
    delete syntheseVocale;
    delete ui;
}

void Gemploye::peuplerStatuts()
{
    ui->comboBox_4->clear();

    QStringList statuts = {
        "Actif",
        "Inactif",
        "En négociation"
    };

    for (const QString &statut : statuts) {
        ui->comboBox_4->addItem(statut);
    }

    ui->comboBox_4->setCurrentText("Actif");
}

void Gemploye::chargerStatuts()
{
    QSettings settings("SparkFlow", "GestionSponsors");

    QStringList keys = settings.allKeys();
    for (const QString &key : keys) {
        if (key.startsWith("statut_")) {
            int codeSponsor = key.mid(7).toInt();
            QString statut = settings.value(key).toString();
            statutsSponsors[codeSponsor] = statut;
        }
    }

    qDebug() << "Statuts chargés:" << statutsSponsors.size() << "sponsors";
}

void Gemploye::sauvegarderStatuts()
{
    QSettings settings("SparkFlow", "GestionSponsors");

    for (auto it = statutsSponsors.begin(); it != statutsSponsors.end(); ++it) {
        QString key = "statut_" + QString::number(it.key());
        settings.setValue(key, it.value());
    }

    qDebug() << "Statuts sauvegardés:" << statutsSponsors.size() << "sponsors";
}

QString Gemploye::getStatutSponsor(int codeSponsor)
{
    if (statutsSponsors.contains(codeSponsor)) {
        return statutsSponsors[codeSponsor];
    }
    return "Actif";
}

void Gemploye::setStatutSponsor(int codeSponsor, const QString &statut)
{
    statutsSponsors[codeSponsor] = statut;
    sauvegarderStatuts();
}

void Gemploye::peuplerSecteursActivite()
{
    ui->comboBox_7->clear();

    QStringList secteurs = {
        "Finance",
        "Technologie & Digital",
        "Industrie & Énergie",
        "Éducation & Formation",
        "Commerce",
        "Médias",
        "Santé"
    };

    ui->comboBox_7->addItem("");

    for (const QString &secteur : secteurs) {
        ui->comboBox_7->addItem(secteur);
    }
}

void Gemploye::on_btn_microphone_clicked()
{
    lancer_analyse_strategique();
}

void Gemploye::lancer_analyse_strategique()
{
    QString requete = ui->lineEdit_35->text().trimmed();

    if (requete.isEmpty()) {
        parler_reponse("Veuillez saisir un budget pour obtenir une analyse stratégique. Par exemple : 5000 euros");
        return;
    }

    QString resultat = analyser_budget(requete);
    parler_reponse(resultat);
}

QString Gemploye::analyser_budget(const QString &requete)
{
    QRegularExpression budget_regex("(\\d+)");
    QRegularExpressionMatchIterator matches = budget_regex.globalMatch(requete);

    if (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        int budget = match.captured(1).toInt();

        if (budget > 0) {
            return generer_conseil_strategique(budget);
        }
    }

    return "Je n'ai pas compris le montant. Veuillez saisir un nombre comme : 5000, 10000, 15000 euros";
}

QString Gemploye::generer_conseil_strategique(int budget)
{
    QVector<QPair<QString, double>> secteurs = analyser_secteurs();

    if (secteurs.isEmpty()) {
        return "Aucune donnée de sponsor disponible pour l'analyse stratégique.";
    }

    QString meilleur_secteur;
    double meilleur_score = -1;
    int nombre_sponsors_meilleur = 0;
    double budget_moyen_meilleur = 0;

    QSqlQuery query;
    for (const auto& secteur : secteurs) {
        QString nom_secteur = secteur.first;

        query.prepare("SELECT COUNT(*), AVG(MONTANT) FROM SPONSOR WHERE SECTEUR_ACTIVITÉ = ?");
        query.addBindValue(nom_secteur);

        if (query.exec() && query.next()) {
            int count = query.value(0).toInt();
            double avg_montant = query.value(1).toDouble();

            double score_proximite = 1.0 / (1.0 + abs(avg_montant - budget) / 1000.0);
            double score_densite = count / 10.0;
            double score_total = score_proximite * 0.6 + score_densite * 0.4;

            if (score_total > meilleur_score) {
                meilleur_score = score_total;
                meilleur_secteur = nom_secteur;
                nombre_sponsors_meilleur = count;
                budget_moyen_meilleur = avg_montant;
            }
        }
    }

    QString conseil = QString("En tant que gestionnaire, je vous recommande de cibler le secteur %1. ")
                          .arg(meilleur_secteur.toUpper());

    if (abs(budget_moyen_meilleur - budget) < 1000) {
        conseil += QString("Le budget moyen de %1€ est très proche de votre montant de %2€. ")
                       .arg((int)budget_moyen_meilleur)
                       .arg(budget);
    } else if (budget_moyen_meilleur > budget) {
        conseil += QString("Avec %1 sponsors actifs et un budget moyen de %2€, ce secteur offre des opportunités de croissance. ")
                       .arg(nombre_sponsors_meilleur)
                       .arg((int)budget_moyen_meilleur);
    } else {
        conseil += QString("Ce secteur compte %1 sponsors avec un budget moyen de %2€, idéal pour maximiser votre impact. ")
                       .arg(nombre_sponsors_meilleur)
                       .arg((int)budget_moyen_meilleur);
    }

    if (meilleur_secteur.contains("technolog", Qt::CaseInsensitive)) {
        conseil += "Le secteur technologique est en forte croissance cette année avec une demande accrue pour l'innovation.";
    }
    else if (meilleur_secteur.contains("santé", Qt::CaseInsensitive) || meilleur_secteur.contains("sante", Qt::CaseInsensitive)) {
        conseil += "La santé est un secteur stable avec des partenariats durables et un fort impact social.";
    }
    else if (meilleur_secteur.contains("sport", Qt::CaseInsensitive)) {
        conseil += "Le secteur sportif offre une excellente visibilité et un engagement communautaire élevé.";
    }
    else if (meilleur_secteur.contains("finance", Qt::CaseInsensitive)) {
        conseil += "Le secteur financier propose des budgets importants et des partenariats stratégiques à long terme.";
    }
    else if (meilleur_secteur.contains("éducation", Qt::CaseInsensitive) || meilleur_secteur.contains("education", Qt::CaseInsensitive)) {
        conseil += "L'éducation offre un impact sociétal fort et des opportunités de partenariats institutionnels.";
    }
    else {
        conseil += "Ce secteur présente un bon équilibre entre investissement et potentiel de développement.";
    }

    return conseil;
}

QVector<QPair<QString, double>> Gemploye::analyser_secteurs()
{
    QVector<QPair<QString, double>> secteurs;

    QSqlQuery query("SELECT SECTEUR_ACTIVITÉ, AVG(MONTANT) FROM SPONSOR GROUP BY SECTEUR_ACTIVITÉ HAVING COUNT(*) > 0");

    while (query.next()) {
        QString secteur = query.value(0).toString();
        if (secteur.isEmpty()) continue;

        double avg_montant = query.value(1).toDouble();
        secteurs.append(qMakePair(secteur, avg_montant));
    }

    return secteurs;
}

void Gemploye::parler_reponse(const QString &reponse)
{
    if (syntheseVocale && syntheseVocale->state() == QTextToSpeech::Ready) {
        syntheseVocale->say(reponse);
    }
}

void Gemploye::on_tabWidget_4_currentChanged(int index)
{
    if (index == 1) {
        afficherStatistiques();
    }
}

void Gemploye::afficherStatistiques()
{
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM SPONSOR");
    int totalSponsors = 0;
    if (query.next()) {
        totalSponsors = query.value(0).toInt();
    }

    if (totalSponsors == 0) {
        QVBoxLayout *layout = new QVBoxLayout(ui->tabWidget_4->widget(1));
        QLabel *messageLabel = new QLabel("📊 Aucune donnée de sponsor disponible\n\nAjoutez des sponsors pour voir les statistiques");
        messageLabel->setStyleSheet("font-size: 16px; color: #7f8c8d; text-align: center;");
        messageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(messageLabel);
    } else {
        afficherHistogrammeStatistiques();
    }
}

void Gemploye::afficherHistogrammeStatistiques()
{
    // Nettoyer le layout existant
    if (ui->tabWidget_4->widget(1)->layout()) {
        QLayoutItem* item;
        while ((item = ui->tabWidget_4->widget(1)->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete ui->tabWidget_4->widget(1)->layout();
    }

    // CRÉATION DU LAYOUT PRINCIPAL HORIZONTAL
    QHBoxLayout *mainLayout = new QHBoxLayout(ui->tabWidget_4->widget(1));
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // =============================================
    // PARTIE GAUCHE : DIAGRAMME (70% de l'espace)
    // =============================================
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(5, 5, 5, 5);
    leftLayout->setSpacing(10);

    // Récupérer les données de la base
    QSqlQuery query;
    query.exec("SELECT SECTEUR_ACTIVITÉ, COUNT(*), SUM(MONTANT) FROM SPONSOR GROUP BY SECTEUR_ACTIVITÉ");

    int totalSponsors = 0;
    double montantTotal = 0;
    QHash<QString, int> secteurCounts;
    QHash<QString, double> secteurMontants;
    QStringList secteurs;

    while (query.next()) {
        QString secteur = query.value(0).toString();
        if (secteur.isEmpty()) secteur = "Non spécifié";
        int count = query.value(1).toInt();
        double montant = query.value(2).toDouble();

        secteurCounts[secteur] = count;
        secteurMontants[secteur] = montant;
        secteurs.append(secteur);
        totalSponsors += count;
        montantTotal += montant;
    }

    // Trier les secteurs par montant décroissant
    std::sort(secteurs.begin(), secteurs.end(),
              [&](const QString &a, const QString &b) {
                  return secteurMontants[a] > secteurMontants[b];
              });

    // CORRECTION : Création de l'histogramme avec un SEUL QBarSet
    QBarSeries *series = new QBarSeries();

    // Palette de couleurs
    QList<QColor> colors = {
        QColor("#3498db"), QColor("#2ecc71"), QColor("#e74c3c"), QColor("#f39c12"),
        QColor("#9b59b6"), QColor("#1abc9c"), QColor("#d35400"), QColor("#c0392b"),
        QColor("#16a085"), QColor("#8e44ad"), QColor("#27ae60"), QColor("#2980b9")
    };

    // CORRECTION CRITIQUE : Créer un SEUL QBarSet pour toutes les barres
    QBarSet *barSet = new QBarSet("Montants par secteur");

    // Ajouter les montants dans le MÊME ordre que les secteurs triés
    for (int i = 0; i < secteurs.count(); ++i) {
        QString secteur = secteurs[i];
        double montant = secteurMontants[secteur];
        *barSet << montant;
    }

    // CORRECTION : Appliquer les couleurs aux barres individuelles
    for (int i = 0; i < secteurs.count(); ++i) {
        barSet->setColor(colors[i % colors.size()]);
    }

    barSet->setLabelColor(Qt::white);
    barSet->setLabelFont(QFont("Arial", 9, QFont::Bold));
    series->append(barSet);

    series->setBarWidth(0.7);
    series->setLabelsVisible(true);
    series->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);
    series->setLabelsFormat("@value €");

    // Création du chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("📊 HISTOGRAMME DES MONTANTS PAR SECTEUR");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->setTitleBrush(QBrush(Qt::black));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundBrush(QBrush(Qt::transparent));
    chart->setBackgroundVisible(false);
    chart->setMargins(QMargins(10, 10, 10, 10));

    // CORRECTION : Configuration de l'axe Y (montants) avec échelle organisée
    QValueAxis *axisY = new QValueAxis();
    double maxMontant = 0;
    for (const QString &currentSecteur : secteurs) {
        if (secteurMontants[currentSecteur] > maxMontant) {
            maxMontant = secteurMontants[currentSecteur];
        }
    }

    // Calculer une échelle bien organisée
    double maxAxisValue = calculerEchelle(maxMontant * 1.1);
    double intervalle = calculerIntervalle(maxAxisValue);

    axisY->setRange(0, maxAxisValue);
    axisY->setTitleText("MONTANT (€)");
    axisY->setTitleFont(QFont("Arial", 11, QFont::Bold));
    axisY->setLabelFormat("%d €"); // Supprime les décimales pour un affichage plus propre
    axisY->setLabelsFont(QFont("Arial", 9));
    axisY->setLabelsColor(Qt::black);
    axisY->setTitleBrush(QBrush(Qt::black));
    axisY->setGridLineVisible(true);
    axisY->setGridLineColor(QColor("#bdc3c7"));

    // Définir un intervalle fixe pour des ticks réguliers
    axisY->setTickCount(6); // 0 + 5 intervalles
    axisY->setTickAnchor(0);
    axisY->setTickInterval(intervalle);

    // CORRECTION : Ajouter l'axe Y d'abord
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // CORRECTION : Configuration de l'axe X (secteurs)
    QBarCategoryAxis *axisX = new QBarCategoryAxis();

    // CORRECTION : S'assurer que l'ordre des catégories correspond à l'ordre des barres
    QStringList categories;
    for (int i = 0; i < secteurs.count(); ++i) {
        categories << secteurs[i];
    }
    axisX->append(categories);

    axisX->setTitleText("SECTEURS D'ACTIVITÉ");
    axisX->setTitleFont(QFont("Arial", 11, QFont::Bold));
    axisX->setLabelsFont(QFont("Arial", 9));
    axisX->setLabelsColor(Qt::black);
    axisX->setTitleBrush(QBrush(Qt::black));

    // CORRECTION : Ajouter l'axe X après l'axe Y
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Légende
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(Qt::black);
    chart->legend()->setFont(QFont("Arial", 9));

    // Vue du chart avec taille adaptative
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent; border: 1px solid #bdc3c7; border-radius: 8px;");
    chartView->setMinimumSize(600, 400);
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    leftLayout->addWidget(chartView);

    // =============================================
    // PARTIE DROITE : STATISTIQUES (30% de l'espace)
    // =============================================
    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(5, 5, 5, 5);
    rightLayout->setSpacing(15);

    // Titre des statistiques
    QLabel *statsTitle = new QLabel("📈 STATISTIQUES DÉTAILLÉES");
    statsTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; padding: 10px; background-color: #ecf0f1; border-radius: 8px;");
    statsTitle->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(statsTitle);

    // Résumé global
    QLabel *resumeLabel = new QLabel();
    resumeLabel->setStyleSheet("font-size: 14px; color: #2c3e50; padding: 15px; background-color: #f8f9fa; border: 2px solid #3498db; border-radius: 8px;");
    resumeLabel->setText(QString(
                             "<div style='text-align: center;'>"
                             "<h3 style='color: #2980b9; margin-bottom: 10px;'>RÉSUMÉ GLOBAL</h3>"
                             "<p style='margin: 8px 0;'><b>🏢 Total Sponsors:</b> %1</p>"
                             "<p style='margin: 8px 0;'><b>💰 Montant Total:</b> %2 €</p>"
                             "<p style='margin: 8px 0;'><b>📊 Secteurs Actifs:</b> %3</p>"
                             "</div>"
                             ).arg(totalSponsors).arg(montantTotal, 0, 'f', 2).arg(secteurCounts.count()));
    rightLayout->addWidget(resumeLabel);

    // Détails par secteur
    QLabel *detailsTitle = new QLabel("📋 DÉTAILS PAR SECTEUR");
    detailsTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; padding: 8px; background-color: #ecf0f1; border-radius: 5px;");
    detailsTitle->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(detailsTitle);

    // Conteneur scrollable pour les détails des secteurs
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setMaximumHeight(300);
    scrollArea->setStyleSheet("background: transparent; border: 1px solid #bdc3c7; border-radius: 8px;");
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(10, 10, 10, 10);
    scrollLayout->setSpacing(8);

    // Ajouter les détails pour chaque secteur
    for (int i = 0; i < secteurs.count(); ++i) {
        QString secteur = secteurs[i];
        double montant = secteurMontants[secteur];
        int count = secteurCounts[secteur];
        double pourcentage = (montant / montantTotal) * 100;

        QFrame *secteurFrame = new QFrame();
        secteurFrame->setStyleSheet(QString(
                                        "QFrame {"
                                        "background-color: %1;"
                                        "border: 1px solid #95a5a6;"
                                        "border-radius: 6px;"
                                        "padding: 8px;"
                                        "}"
                                        ).arg(colors[i % colors.size()].lighter(130).name()));

        QHBoxLayout *secteurLayout = new QHBoxLayout(secteurFrame);
        secteurLayout->setContentsMargins(5, 5, 5, 5);

        QLabel *secteurInfo = new QLabel();
        secteurInfo->setStyleSheet("font-size: 12px; color: #2c3e50; font-weight: bold;");
        secteurInfo->setText(QString(
                                 "<b>%1</b><br>"
                                 "Sponsors: %2<br>"
                                 "Montant: %3 €<br>"
                                 "Part: %4%"
                                 ).arg(secteur).arg(count).arg((int)montant).arg(pourcentage, 0, 'f', 1));

        secteurLayout->addWidget(secteurInfo);
        scrollLayout->addWidget(secteurFrame);
    }

    // Étirer pour prendre l'espace restant
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    rightLayout->addWidget(scrollArea);

    // Indicateurs de performance
    QLabel *performanceTitle = new QLabel("🎯 INDICATEURS DE PERFORMANCE");
    performanceTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; padding: 8px; background-color: #ecf0f1; border-radius: 5px;");
    performanceTitle->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(performanceTitle);

    QLabel *performanceLabel = new QLabel();
    performanceLabel->setStyleSheet("font-size: 12px; color: #2c3e50; padding: 10px; background-color: #f8f9fa; border: 1px solid #bdc3c7; border-radius: 8px;");

    // Calculer quelques indicateurs
    double moyenneParSponsor = montantTotal / totalSponsors;
    QString secteurPrincipal = secteurs.count() > 0 ? secteurs[0] : "Aucun";
    double montantSecteurPrincipal = secteurs.count() > 0 ? secteurMontants[secteurs[0]] : 0;

    performanceLabel->setText(QString(
                                  "<div style='text-align: left;'>"
                                  "<p style='margin: 5px 0;'>📈 <b>Moyenne/Sponsor:</b> %1 €</p>"
                                  "<p style='margin: 5px 0;'>🏆 <b>Secteur Principal:</b> %2</p>"
                                  "<p style='margin: 5px 0;'>💎 <b>Montant Principal:</b> %3 €</p>"
                                  "<p style='margin: 5px 0;'>📅 <b>Dernière MAJ:</b> %4</p>"
                                  "</div>"
                                  ).arg(moyenneParSponsor, 0, 'f', 2)
                                  .arg(secteurPrincipal)
                                  .arg((int)montantSecteurPrincipal)
                                  .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm")));

    rightLayout->addWidget(performanceLabel);

    // Ajouter les deux parties au layout principal
    mainLayout->addWidget(leftWidget, 7);  // 70% pour le diagramme
    mainLayout->addWidget(rightWidget, 3); // 30% pour les statistiques
}

double Gemploye::calculerEchelle(double valeur)
{
    if (valeur <= 0) return 1000;

    // Arrondir à la valeur supérieure la plus proche (100, 500, 1000, 5000, etc.)
    double exponent = floor(log10(valeur));
    double fraction = valeur / pow(10, exponent);

    double scale = 1.0;
    if (fraction <= 2.0) {
        scale = 2.0;
    } else if (fraction <= 5.0) {
        scale = 5.0;
    } else {
        scale = 10.0;
    }

    return scale * pow(10, exponent);
}

double Gemploye::calculerIntervalle(double maxValue)
{
    if (maxValue <= 0) return 1000;

    // Déterminer un intervalle approprié basé sur la valeur max
    if (maxValue <= 1000) {
        return 200;
    } else if (maxValue <= 5000) {
        return 1000;
    } else if (maxValue <= 10000) {
        return 2000;
    } else if (maxValue <= 50000) {
        return 10000;
    } else if (maxValue <= 100000) {
        return 20000;
    } else {
        return 50000;
    }
}

void Gemploye::setupSponsorsValidators()
{
    QRegularExpressionValidator *codeValidator = new QRegularExpressionValidator(QRegularExpression("[0-9]{1,10}"), this);
    ui->lineEdit_21->setValidator(codeValidator);
    ui->lineEdit_26->setValidator(codeValidator);

    QRegularExpressionValidator *nomValidator = new QRegularExpressionValidator(QRegularExpression("[a-zA-ZÀ-ÿ '-]{2,50}"), this);
    ui->lineEdit_22->setValidator(nomValidator);

    QRegularExpressionValidator *montantValidator = new QRegularExpressionValidator(QRegularExpression("[0-9]*[.]?[0-9]{0,2}"), this);
    ui->lineEdit_24->setValidator(montantValidator);
}

bool Gemploye::codeExistsInDatabase(int code)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM SPONSOR WHERE CODE_UNIQUE = :code OR ID_SPONSOR = :code");
    query.bindValue(":code", code);

    if(query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool Gemploye::isValidName(const QString &name)
{
    if(name.length() < 2 || name.length() > 50) return false;
    QRegularExpression regex("^[a-zA-ZÀ-ÿ]+([ '-][a-zA-ZÀ-ÿ]+)*$");
    return regex.match(name).hasMatch();
}

bool Gemploye::isValidSecteur(const QString &secteur)
{
    Q_UNUSED(secteur);
    return true;
}

bool Gemploye::isValidMontantFormat(const QString &montant)
{
    QRegularExpression regex("^[0-9]+([.][0-9]{1,2})?$");
    return regex.match(montant).hasMatch();
}

void Gemploye::highlightEmptyFields(const QString &code, const QString &nom)
{
    resetFieldStyles();
    if(code.isEmpty()) ui->lineEdit_21->setStyleSheet("border: 2px solid red;");
    if(nom.isEmpty()) ui->lineEdit_22->setStyleSheet("border: 2px solid red;");
    if(code.isEmpty()) ui->lineEdit_21->setFocus();
    else if(nom.isEmpty()) ui->lineEdit_22->setFocus();
}

void Gemploye::resetFieldStyles()
{
    ui->lineEdit_21->setStyleSheet("");
    ui->lineEdit_22->setStyleSheet("");
    ui->lineEdit_24->setStyleSheet("");
    ui->lineEdit_26->setStyleSheet("");
}

void Gemploye::clearSponsorFields()
{
    ui->lineEdit_21->clear();
    ui->lineEdit_22->clear();
    ui->comboBox_7->setCurrentIndex(0);
    ui->lineEdit_24->clear();
    ui->comboBox_4->setCurrentText("Actif");
    modeModification = false;
    idSponsorEnCours = -1;
    ui->pushButton_15->setText("Valider");
}

void Gemploye::actualiserTableauSponsors()
{
    QSqlQueryModel *model = sponsor.afficher();
    if (!model) {
        qDebug() << "Erreur: modèle null dans actualiserTableauSponsors";
        return;
    }

    ui->tableWidget_4->setRowCount(model->rowCount());
    ui->tableWidget_4->setColumnCount(5);

    QStringList headers;
    headers << "Code Unique" << "Nom Sponsor" << "Secteur Activité" << "Montant" << "Statut";
    ui->tableWidget_4->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 1; col < 5; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(
                model->data(model->index(row, col)).toString()
                );
            ui->tableWidget_4->setItem(row, col-1, item);
        }

        int codeSponsor = model->data(model->index(row, 1)).toInt();
        QString statut = getStatutSponsor(codeSponsor);
        QTableWidgetItem *statutItem = new QTableWidgetItem(statut);
        ui->tableWidget_4->setItem(row, 4, statutItem);
    }

    ui->tableWidget_4->resizeColumnsToContents();
    delete model;
}

void Gemploye::chargerSponsorPourModification(int row)
{
    if (row < 0 || row >= ui->tableWidget_4->rowCount()) return;

    QString codeText = ui->tableWidget_4->item(row, 0)->text();
    int code = codeText.toInt();

    QSqlQuery query;
    query.prepare("SELECT ID_SPONSOR FROM SPONSOR WHERE CODE_UNIQUE = ?");
    query.addBindValue(code);

    int id = -1;
    if (query.exec() && query.next()) {
        id = query.value(0).toInt();
    }

    if (id == -1) {
        QMessageBox::warning(this, "Erreur", "Impossible de trouver le sponsor sélectionné.");
        return;
    }

    QString nom = ui->tableWidget_4->item(row, 1)->text();
    QString secteur = ui->tableWidget_4->item(row, 2)->text();
    QString montant = ui->tableWidget_4->item(row, 3)->text();
    QString statut = ui->tableWidget_4->item(row, 4)->text();

    ui->lineEdit_21->setText(codeText);
    ui->lineEdit_22->setText(nom);

    int indexSecteur = ui->comboBox_7->findText(secteur);
    if (indexSecteur != -1) {
        ui->comboBox_7->setCurrentIndex(indexSecteur);
    } else {
        ui->comboBox_7->setCurrentIndex(0);
    }

    ui->lineEdit_24->setText(montant);

    int indexStatut = ui->comboBox_4->findText(statut);
    if (indexStatut != -1) {
        ui->comboBox_4->setCurrentIndex(indexStatut);
    } else {
        ui->comboBox_4->setCurrentText("Actif");
    }

    modeModification = true;
    idSponsorEnCours = id;
    ui->pushButton_15->setText("Modifier");

    QMessageBox::information(this, "Mode Modification",
                             "Sponsor chargé pour modification.\nCliquez sur 'Modifier' pour sauvegarder les changements.");
}

void Gemploye::annulerModification()
{
    clearSponsorFields();
    QMessageBox::information(this, "Annulation", "Mode modification annulé.");
}

void Gemploye::on_tableWidget_4_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    chargerSponsorPourModification(row);
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

void Gemploye::on_pushButton_15_clicked()
{
    QString idText = ui->lineEdit_21->text();
    QString nom = ui->lineEdit_22->text();
    QString secteur = ui->comboBox_7->currentText();
    QString montantText = ui->lineEdit_24->text();
    QString statut = ui->comboBox_4->currentText();

    if(idText.isEmpty() || nom.isEmpty()) {
        QMessageBox::warning(this, "Champs obligatoires",
                             "Les champs Code Unique et Nom sont obligatoires.");
        highlightEmptyFields(idText, nom);
        return;
    }

    bool okCode;
    int code = idText.toInt(&okCode);

    if(!okCode || code < 0) {
        QMessageBox::warning(this, "Code Unique invalide",
                             "Le Code Unique doit être un nombre entier positif ou zéro.");
        ui->lineEdit_21->setStyleSheet("border: 2px solid red;");
        ui->lineEdit_21->setFocus();
        return;
    }

    if(!isValidName(nom)) {
        QMessageBox::warning(this, "Nom invalide",
                             "Le nom du sponsor est invalide.\n"
                             "• Doit contenir entre 2 et 50 caractères\n"
                             "• Ne peut contenir que des lettres, espaces, apostrophes et tirets\n"
                             "• Doit commencer par une lettre");
        ui->lineEdit_22->setStyleSheet("border: 2px solid red;");
        ui->lineEdit_22->setFocus();
        return;
    }

    float montant = 0.0;
    if(!montantText.isEmpty()) {
        bool okMontant;
        montant = montantText.toFloat(&okMontant);

        if(!okMontant || montant < 0) {
            QMessageBox::warning(this, "Montant invalide",
                                 "Le montant doit être un nombre positif (ex: 1500.50).");
            ui->lineEdit_24->setStyleSheet("border: 2px solid red;");
            ui->lineEdit_24->setFocus();
            return;
        }

        if(!isValidMontantFormat(montantText)) {
            QMessageBox::warning(this, "Format de montant invalide",
                                 "Le format du montant est incorrect.\n"
                                 "Exemples valides : 1500, 1500.5, 1500.50\n"
                                 "Exemples invalides : 1500.123, 1,500.50");
            ui->lineEdit_24->setStyleSheet("border: 2px solid red;");
            ui->lineEdit_24->setFocus();
            return;
        }
    }

    resetFieldStyles();

    bool test = false;
    QString message = "";

    if (modeModification) {
        Gestion_Sponsor s(idSponsorEnCours, code, nom, secteur, montant);
        test = s.modifier(idSponsorEnCours);

        if (test) {
            setStatutSponsor(code, statut);
        }

        message = "Sponsor modifié avec succès!";
    } else {
        QSqlQuery query;
        query.exec("SELECT MAX(ID_SPONSOR) FROM SPONSOR");
        int maxId = 0;
        if (query.next()) {
            maxId = query.value(0).toInt();
        }
        int newId = maxId + 1;

        Gestion_Sponsor s(newId, code, nom, secteur, montant);
        test = s.ajouter();

        if (test) {
            setStatutSponsor(code, statut);
        }

        message = "Sponsor ajouté avec succès!";
    }

    if(test) {
        actualiserTableauSponsors();
        QMessageBox::information(this, "Succès",
                                 QString("%1\n\n"
                                         "Code Unique: %2\n"
                                         "Nom: %3\n"
                                         "Secteur: %4\n"
                                         "Montant: %5 €\n"
                                         "Statut: %6")
                                     .arg(message)
                                     .arg(code)
                                     .arg(nom)
                                     .arg(secteur.isEmpty() ? "Non spécifié" : secteur)
                                     .arg(montant)
                                     .arg(statut));
        clearSponsorFields();
    } else {
        QMessageBox::critical(this, "Erreur",
                              QString("Erreur lors de %1 du sponsor.\n"
                                      "Vérifiez la connexion à la base de données.")
                                  .arg(modeModification ? "la modification" : "l'ajout"));
    }
}

void Gemploye::on_pushButton_18_clicked()
{
    QString codeText = ui->lineEdit_26->text();

    if(codeText.isEmpty()) {
        QMessageBox::warning(this, "Champ empty",
                             "Veuillez saisir un Code Unique de sponsor à supprimer.");
        ui->lineEdit_26->setStyleSheet("border: 2px solid red;");
        ui->lineEdit_26->setFocus();
        return;
    }

    bool okCode;
    int code = codeText.toInt(&okCode);

    if(!okCode || code < 0) {
        QMessageBox::warning(this, "Code invalide",
                             "Le Code Unique doit être un nombre entier positif ou zéro.");
        ui->lineEdit_26->setStyleSheet("border: 2px solid red;");
        ui->lineEdit_26->setFocus();
        return;
    }

    if(!codeExistsInDatabase(code)) {
        QMessageBox::warning(this, "Sponsor non trouvé",
                             "Aucun sponsor trouvé avec ce Code Unique.");
        ui->lineEdit_26->setStyleSheet("border: 2px solid orange;");
        ui->lineEdit_26->setFocus();
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation de suppression",
                                  QString("Êtes-vous sûr de vouloir supprimer le sponsor avec le code %1 ?\n\n"
                                          "Cette action est irréversible.")
                                      .arg(code),
                                  QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::No) {
        return;
    }

    resetFieldStyles();
    bool test = sponsor.supprimer(code);

    if(test) {
        if (statutsSponsors.contains(code)) {
            statutsSponsors.remove(code);
            sauvegarderStatuts();
        }

        actualiserTableauSponsors();
        QMessageBox::information(this, "Succès", "Sponsor supprimé avec succès!");
        ui->lineEdit_26->clear();
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Erreur lors de la suppression du sponsor.\n"
                              "Vérifiez qu'aucun contrat n'est lié à ce sponsor.");
    }
}

void Gemploye::on_pushButton_16_clicked()
{
    if (modeModification) {
        annulerModification();
    } else {
        resetFieldStyles();
        clearSponsorFields();
        QMessageBox::information(this, "Annulation", "Saisie annulée. Tous les champs ont été vidés.");
    }
}


void Gemploye::on_pushButton_17_clicked()
{
    // Demander où sauvegarder le PDF
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer la fiche sponsor", QDir::homePath() + "/Fiche_Sponsor.pdf", "Fichiers PDF (*.pdf)");

    if (fileName.isEmpty()) {
        return;
    }

    // Créer le PDF Writer
    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageOrientation(QPageLayout::Portrait);
    pdfWriter.setTitle("Fiche de Suivi Sponsor");
    pdfWriter.setCreator("Gestion Sponsors App");
    pdfWriter.setResolution(150);

    QPainter painter;
    if (!painter.begin(&pdfWriter)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le PDF");
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Dimensions en millimètres - AUGMENTER LES MARGES
    int margin = 30;
    int contentWidth = pdfWriter.width() - 2 * margin;
    int currentY = margin;

    // ==================== EN-TÊTE ====================
    QFont titleFont("Arial", 20, QFont::Bold);
    painter.setFont(titleFont);
    painter.setPen(QColor(41, 128, 185));

    painter.drawText(QRect(margin, currentY, contentWidth, 40), Qt::AlignCenter, "FICHE DE SUIVI SPONSOR");
    currentY += 60;

    // Ligne de séparation
    painter.setPen(QPen(QColor(41, 128, 185), 1));
    painter.drawLine(margin, currentY, margin + contentWidth, currentY);
    currentY += 30;


    // ==================== INFORMATIONS SPONSOR ====================
    drawSectionHeader(painter, "INFORMATIONS SPONSOR", margin, currentY, contentWidth);
    currentY += 70;

    QFont fieldFont("Arial", 11);
    painter.setFont(fieldFont);
    painter.setPen(Qt::black);

    painter.drawText(margin + 15, currentY, "CODE: _____________       PARTENAIRE DEPUIS: _____________");
    currentY += 45;

    painter.drawText(margin + 15, currentY, "CONTACT: ___________________________________________________");
    currentY += 45;

    painter.drawText(margin + 15, currentY, "SECTEUR: ___________________________________________________");
    currentY += 45;

    painter.drawText(margin + 15, currentY, "STATUT: ___________________________________________________");
    currentY += 60;

    // ==================== HISTORIQUE FINANCIER ====================
    drawSectionHeader(painter, "HISTORIQUE FINANCIER", margin, currentY, contentWidth);
    currentY += 70;

    // En-tête du tableau
    QFont tableHeaderFont("Arial", 11, QFont::Bold);
    painter.setFont(tableHeaderFont);
    painter.setPen(QColor(41, 128, 185));

    int col1 = margin + 60;
    int col2 = col1 + 120;

    painter.drawText(col1, currentY, "DATE      |");
    painter.drawText(col2, currentY, "MONTANT");
    currentY += 20;

    // Ligne de séparation
    painter.setPen(QPen(Qt::gray, 1));
    painter.drawLine(margin + 10, currentY, margin + contentWidth - 10, currentY);
    currentY += 20;

    // Lignes du tableau
    painter.setFont(fieldFont);
    painter.setPen(Qt::black);

    for (int i = 0; i < 5; i++) {
        painter.drawText(col1, currentY, "__________  |");
        painter.drawText(col2, currentY, "____________ €");
        currentY += 35;
    }

    // Total
    currentY += 20;
    painter.setFont(tableHeaderFont);
    painter.setPen(QColor(52, 73, 94));
    painter.drawText(col1, currentY, "TOTAL VERSÉ:");
    painter.setFont(fieldFont);
    painter.drawText(col2, currentY, "       ___________________ €");
    currentY += 60;

    // ==================== PROCHAINES ACTIONS ====================
    drawSectionHeader(painter, "PROCHAINES ACTIONS", margin, currentY, contentWidth);
    currentY += 70;

    painter.setFont(fieldFont);
    painter.setPen(Qt::black);

    for (int i = 0; i < 3; i++) {
        painter.drawText(margin + 40, currentY, "Date: ____/____/____");
        painter.drawText(margin + 300, currentY, "Action: ___________________________________________");
        currentY += 50;
    }
    currentY += 40;

    // ==================== NOTES ====================
    drawSectionHeader(painter, "NOTES & OBSERVATIONS", margin, currentY, contentWidth);
    currentY += 50;

    QRect notesRect(margin + 10, currentY, contentWidth - 20, 200);
    painter.setPen(QPen(Qt::gray, 1));
    painter.setBrush(QColor(240, 240, 240));
    painter.drawRect(notesRect);


    painter.setFont(QFont("Arial", 10));
    painter.setPen(Qt::darkGray);

    // ==================== PIED DE PAGE ====================
    painter.setFont(QFont("Arial", 8));
    painter.setPen(Qt::gray);

    painter.drawText(QRect(margin, pdfWriter.height() - 40, contentWidth, 15),
                     Qt::AlignCenter, "Généré le " + QDateTime::currentDateTime().toString("dd/MM/yyyy"));

    painter.drawText(QRect(margin, pdfWriter.height() - 25, contentWidth, 15),
                     Qt::AlignCenter, "Document confidentiel - Gestion Sponsors App - Page 1/1");

    QMessageBox::information(this, "PDF Généré",
                             "Fiche de suivi sponsor générée avec succès!\n\n"
                             "Fichier: " + fileName);
}

void Gemploye::drawSectionHeader(QPainter &painter, const QString &title, int x, int y, int width)
{
    QFont headerFont("Arial", 14, QFont::Bold);
    painter.setFont(headerFont);

    // Fond de section
    painter.setBrush(QColor(41, 128, 185));
    painter.setPen(Qt::NoPen);
    painter.drawRect(x, y, width, 30);

    // Texte
    painter.setPen(Qt::white);
    painter.drawText(QRect(x + 15, y, width - 30, 30), Qt::AlignLeft, title);

    // Icône
    painter.setFont(QFont("Arial", 12));
    painter.drawText(QRect(x + width - 30, y, 20, 30), Qt::AlignCenter, "➤");
}


void Gemploye::on_lineEdit_25_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()) {
        actualiserTableauSponsors();
    } else {
        QSqlQueryModel *model = sponsor.rechercher(arg1);
        if (!model) return;

        ui->tableWidget_4->setRowCount(model->rowCount());
        ui->tableWidget_4->setColumnCount(5);

        for (int row = 0; row < model->rowCount(); ++row) {
            for (int col = 1; col < 5; ++col) {
                QTableWidgetItem *item = new QTableWidgetItem(
                    model->data(model->index(row, col)).toString()
                    );
                ui->tableWidget_4->setItem(row, col-1, item);
            }

            int codeSponsor = model->data(model->index(row, 1)).toInt();
            QString statut = getStatutSponsor(codeSponsor);
            QTableWidgetItem *statutItem = new QTableWidgetItem(statut);
            ui->tableWidget_4->setItem(row, 4, statutItem);
        }
        ui->tableWidget_4->resizeColumnsToContents();
        delete model;
    }
}

void Gemploye::on_comboBox_5_currentTextChanged(const QString &arg1)
{
    QSqlQueryModel *model = nullptr;

    if(arg1 == "secteur") {
        model = sponsor.trier("SECTEUR_ACTIVITÉ ASC");
    } else if(arg1 == "montant") {
        model = sponsor.trier("MONTANT DESC");
    } else if(arg1 == "code unique") {
        model = sponsor.trier("CODE_UNIQUE ASC");
    } else {
        model = sponsor.trier("ID_SPONSOR ASC");
    }

    if(model) {
        ui->tableWidget_4->setRowCount(model->rowCount());
        ui->tableWidget_4->setColumnCount(5);

        for (int row = 0; row < model->rowCount(); ++row) {
            for (int col = 1; col < 5; ++col) {
                QTableWidgetItem *item = new QTableWidgetItem(
                    model->data(model->index(row, col)).toString()
                    );
                ui->tableWidget_4->setItem(row, col-1, item);
            }

            int codeSponsor = model->data(model->index(row, 1)).toInt();
            QString statut = getStatutSponsor(codeSponsor);
            QTableWidgetItem *statutItem = new QTableWidgetItem(statut);
            ui->tableWidget_4->setItem(row, 4, statutItem);
        }
        ui->tableWidget_4->resizeColumnsToContents();
        delete model;
    }
}

void Gemploye::on_lineEdit_21_textChanged(const QString &text)
{
    if(text.isEmpty()) {
        ui->lineEdit_21->setStyleSheet("");
        return;
    }

    bool ok;
    int value = text.toInt(&ok);

    if(!ok || value < 0) {
        ui->lineEdit_21->setStyleSheet("border: 2px solid red;");
    } else {
        ui->lineEdit_21->setStyleSheet("border: 2px solid green;");
    }
}

void Gemploye::on_lineEdit_22_textChanged(const QString &text)
{
    if(text.isEmpty()) {
        ui->lineEdit_22->setStyleSheet("");
        return;
    }

    if(isValidName(text)) {
        ui->lineEdit_22->setStyleSheet("border: 2px solid green;");
    } else {
        ui->lineEdit_22->setStyleSheet("border: 2px solid red;");
    }
}

void Gemploye::on_lineEdit_24_textChanged(const QString &text)
{
    if(text.isEmpty()) {
        ui->lineEdit_24->setStyleSheet("");
        return;
    }

    if(isValidMontantFormat(text)) {
        ui->lineEdit_24->setStyleSheet("border: 2px solid green;");
    } else {
        ui->lineEdit_24->setStyleSheet("border: 2px solid red;");
    }
}

void Gemploye::on_lineEdit_26_textChanged(const QString &text)
{
    if(text.isEmpty()) {
        ui->lineEdit_26->setStyleSheet("");
        return;
    }

    bool ok;
    int value = text.toInt(&ok);

    if(!ok || value < 0) {
        ui->lineEdit_26->setStyleSheet("border: 2px solid red;");
    } else if(!codeExistsInDatabase(value)) {
        ui->lineEdit_26->setStyleSheet("border: 2px solid orange;");
    } else {
        ui->lineEdit_26->setStyleSheet("border: 2px solid green;");
    }
}
