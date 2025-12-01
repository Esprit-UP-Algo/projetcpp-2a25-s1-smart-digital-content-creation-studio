#include "gemploye.h"
#include "ui_gemploye.h"

#include <QAbstractItemView>
#include <QDate>
#include <QHeaderView>
#include <QLocale>
#include <QMessageBox>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QStringList>
#include <QVector>
#include <QVBoxLayout>
#include <QPainter>
#include <QMargins>
#include <QBrush>
#include <memory>

#ifdef HAVE_CHARTS
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLegend>
#endif
#include <QScrollArea>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QFileDialog>
#include <QDateTime>
#include <QPageSize>
#include <QPageLayout>

Gemploye::Gemploye(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gemploye)
{
    ui->setupUi(this);

    QPixmap logo(":/images/logo.png");
    ui->label_7->setPixmap(logo.scaled(ui->label_7->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    
    // Install event filter on logo to make it clickable
    ui->label_7->installEventFilter(this);
    ui->label_7->setMouseTracking(true);
    
    // Initialize Arduino
    arduino = new Arduino();
    
    // Initialize QR Code Generator
    qrGenerator = new QRCodeGenerator(this);
    
    // Initialize System Tray Icon for Windows notifications
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setIcon(QIcon(":/images/logo.png")); // Use your app icon
    systemTrayIcon->setToolTip(tr("Project Management System"));
    systemTrayIcon->show();
}

Gemploye::~Gemploye()
{
    if (arduino)
    {
        arduino->close_arduino();
        delete arduino;
    }
    delete ui;
}

void Gemploye::initAfterConnect()
{
#ifdef HAVE_CHARTS
    if (!chartViewProjet && ui->widget)
    {
        auto *layout = new QVBoxLayout(ui->widget);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(0);

        chartViewProjet = new QChartView(new QChart(), ui->widget);
        chartViewProjet->setRenderHint(QPainter::Antialiasing);
        chartViewProjet->setMinimumSize(400, 280);
        chartViewProjet->setStyleSheet(QStringLiteral(
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
            " stop:0 #ffffff, stop:1 #f2f5ff);"
            " border-radius: 16px;"
            " border: 1px solid rgba(42, 23, 76, 40);"));

        layout->addWidget(chartViewProjet);

        auto *chart = chartViewProjet->chart();
        chart->setBackgroundRoundness(16);
        chart->setMargins(QMargins(16, 16, 16, 16));
        chart->setTitleBrush(QBrush(QColor("#2a174c")));
        chart->legend()->setLabelColor(Qt::black);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->setDropShadowEnabled(true);
    }
#endif

    initialiserProjetUi();
    rafraichirStatistiquesProjet();
    
    // Test Arduino connection on startup
    testArduinoConnection();
    
    // Add QR Code menu
    QMenu *qrMenu = menuBar()->addMenu(tr("&QR Code"));
    QAction *generateAction = qrMenu->addAction(tr("&Generate QR Code for Project"));
    QAction *scanAction = qrMenu->addAction(tr("&Scan QR Code"));
    
    connect(generateAction, &QAction::triggered, this, [this]() {
        on_generateQRCode_clicked();
    });
    
    connect(scanAction, &QAction::triggered, this, [this]() {
        on_scanQRCode_clicked();
    });
}

void Gemploye::initialiserProjetUi()
{
    if (!ui->tableWidget_2)
    {
        return;
    }

    if (ui->comboBox_2)
    {
        ui->comboBox_2->clear();
        ui->comboBox_2->addItem(tr("Code (A-Z)"));
        ui->comboBox_2->addItem(tr("Titre (A-Z)"));
        ui->comboBox_2->addItem(tr("Budget (desc)"));
        ui->comboBox_2->addItem(tr("Deadline (A-Z)"));
    }

    if (ui->dateEdit_4)
    {
        ui->dateEdit_4->setCalendarPopup(true);
        ui->dateEdit_4->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
        ui->dateEdit_4->setDate(QDate::currentDate());
    }

    ui->tableWidget_2->setColumnCount(4);
    ui->tableWidget_2->setHorizontalHeaderLabels(
        {tr("Code"), tr("Titre"), tr("Budget"), tr("Deadline")});
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Initialize notification button if it exists
    if (ui->notificationButton)
    {
        ui->notificationButton->setText(QStringLiteral("🔔"));
        ui->notificationButton->setStyleSheet(QStringLiteral(
            "QPushButton {"
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #5A6BF2, stop:1 #3452c9);"
            "    color: white;"
            "    border: none;"
            "    border-radius: 20px;"
            "    padding: 8px 16px;"
            "    font-weight: bold;"
            "    font-size: 14px;"
            "    min-width: 40px;"
            "    min-height: 40px;"
            "}"
            "QPushButton:hover {"
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4a5ae8, stop:1 #2a3fc7);"
            "}"
            "QPushButton:pressed {"
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2a3fc7, stop:1 #1a2fa5);"
            "}"));
    }

    // Initialize notification panel if it exists
    if (ui->notificationPanel)
    {
        ui->notificationPanel->setVisible(false);
        QWidget* contentWidget = ui->notificationPanel->widget();
        if (!contentWidget)
        {
            contentWidget = ui->notificationPanel->findChild<QWidget*>("scrollAreaWidgetContents");
            if (contentWidget)
            {
                ui->notificationPanel->setWidget(contentWidget);
            }
            else
            {
                contentWidget = new QWidget();
                contentWidget->setObjectName("scrollAreaWidgetContents");
                contentWidget->setStyleSheet("background-color: #ffffff;");
                ui->notificationPanel->setWidget(contentWidget);
            }
        }
        
        if (contentWidget)
        {
            contentWidget->setStyleSheet("background-color: #ffffff;");
            QVBoxLayout* layout = new QVBoxLayout(contentWidget);
            layout->setSpacing(8);
            layout->setContentsMargins(8, 8, 8, 8);
            contentWidget->setLayout(layout);
        }
    }

    // Initialize project form scroll area
    if (ui->scrollArea_projetForm)
    {
        // Find the content widget
        QWidget* scrollContent = ui->scrollArea_projetForm->findChild<QWidget*>("scrollAreaWidgetContents_projetForm");
        if (scrollContent)
        {
            // Set it as the scroll area's widget if not already set
            if (ui->scrollArea_projetForm->widget() != scrollContent)
            {
                ui->scrollArea_projetForm->setWidget(scrollContent);
            }
            // Ensure the content widget has a minimum height to enable scrolling
            scrollContent->setMinimumHeight(700);
            // Make sure scroll area is configured correctly
            ui->scrollArea_projetForm->setWidgetResizable(true);
            ui->scrollArea_projetForm->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            ui->scrollArea_projetForm->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            // Force update
            scrollContent->updateGeometry();
            ui->scrollArea_projetForm->updateGeometry();
        }
    }

    // Connect form fields to QR code generation for real-time updates
    if (ui->lineEdit_44)
    {
        connect(ui->lineEdit_44, &QLineEdit::textChanged, this, &Gemploye::mettreAJourQRCode);
    }
    if (ui->lineEdit_38)
    {
        connect(ui->lineEdit_38, &QLineEdit::textChanged, this, &Gemploye::mettreAJourQRCode);
    }
    if (ui->lineEdit_39)
    {
        connect(ui->lineEdit_39, &QLineEdit::textChanged, this, &Gemploye::mettreAJourQRCode);
    }
    if (ui->dateEdit_4)
    {
        connect(ui->dateEdit_4, &QDateEdit::dateChanged, this, &Gemploye::mettreAJourQRCode);
    }

    chargerTableProjets();
    viderFormulaireProjet();
}

void Gemploye::chargerTableProjets(QSqlQueryModel* model)
{
    std::unique_ptr<QSqlQueryModel> modelePossede;

    if (!model)
    {
        modelePossede.reset(projetCourant.afficher());
        model = modelePossede.get();
    }

    if (!model)
    {
        ui->tableWidget_2->clearContents();
        ui->tableWidget_2->setRowCount(0);
        return;
    }

    ui->tableWidget_2->clearContents();
    ui->tableWidget_2->setRowCount(model->rowCount());
    ui->tableWidget_2->setColumnCount(model->columnCount());

    QStringList entetes;
    for (int colonne = 0; colonne < model->columnCount(); ++colonne)
    {
        entetes << model->headerData(colonne, Qt::Horizontal).toString();
    }
    ui->tableWidget_2->setHorizontalHeaderLabels(entetes);

    const QLocale locale = QLocale::system();

    for (int ligne = 0; ligne < model->rowCount(); ++ligne)
    {
        for (int colonne = 0; colonne < model->columnCount(); ++colonne)
        {
            const QModelIndex index = model->index(ligne, colonne);
            const QVariant data = model->data(index);
            QString texte = data.toString();

            if (colonne == 2)
            {
                texte = locale.toString(data.toDouble(), 'f', 2);
            }
            else if (colonne == 3)
            {
                QDate date = data.toDate();
                if (!date.isValid())
                {
                    date = QDate::fromString(data.toString().left(10), Qt::ISODate);
                }
                if (date.isValid())
                {
                    texte = date.toString(Qt::ISODate);
                }
            }

            auto* item = new QTableWidgetItem(texte);
            item->setTextAlignment(
                colonne == 2 ? Qt::AlignRight | Qt::AlignVCenter : Qt::AlignLeft | Qt::AlignVCenter);
            ui->tableWidget_2->setItem(ligne, colonne, item);
        }
    }

    rafraichirStatistiquesProjet();
    verifierDeadlinesProjets();
}

void Gemploye::viderFormulaireProjet()
{
    if (ui->lineEdit_44) ui->lineEdit_44->clear();
    if (ui->lineEdit_38) ui->lineEdit_38->clear();
    if (ui->lineEdit_39) ui->lineEdit_39->clear();
    if (ui->lineEdit_34) ui->lineEdit_34->clear();
    if (ui->dateEdit_4) ui->dateEdit_4->setDate(QDate::currentDate());
    if (ui->tableWidget_2) ui->tableWidget_2->clearSelection();
    
    // Clear QR code
    if (ui->qrCodeLabel)
    {
        ui->qrCodeLabel->setText(tr("QR Code"));
        ui->qrCodeLabel->setPixmap(QPixmap());
    }
}

bool Gemploye::verifierChampsProjet(QString& message) const
{
    const QString code = ui->lineEdit_44->text().trimmed();
    const QString titre = ui->lineEdit_38->text().trimmed();
    const QString budgetTexte = ui->lineEdit_39->text().trimmed();
    bool budgetOk = false;
    const double budget = budgetTexte.toDouble(&budgetOk);
    const QDate date = ui->dateEdit_4->date();

    if (code.isEmpty() || titre.isEmpty() || budgetTexte.isEmpty())
    {
        message = tr("Veuillez remplir tous les champs obligatoires (code, titre, budget).");
        return false;
    }

    if (!budgetOk || budget < 0.0)
    {
        message = tr("Le budget doit être un nombre positif.");
        return false;
    }

    if (!date.isValid())
    {
        message = tr("Veuillez sélectionner une date valide.");
        return false;
    }

    message.clear();
    return true;
}

void Gemploye::afficherMessageProjet(const QString& message, QMessageBox::Icon icon)
{
    QMessageBox msgBox(icon,
                       tr("Gestion des projets"),
                       message,
                       QMessageBox::Ok,
                       this);
    msgBox.exec();
}

void Gemploye::remplirFormulaireDepuisLigne(int row)
{
    if (!ui->tableWidget_2 || row < 0 || row >= ui->tableWidget_2->rowCount())
    {
        return;
    }

    const QLocale locale = QLocale::system();

    const QString code = ui->tableWidget_2->item(row, 0)->text();
    const QString titre = ui->tableWidget_2->item(row, 1)->text();
    const QString budgetTexte = ui->tableWidget_2->item(row, 2)->text();
    const QString deadlineTexte = ui->tableWidget_2->item(row, 3)->text();

    if (ui->lineEdit_44) ui->lineEdit_44->setText(code);
    if (ui->lineEdit_38) ui->lineEdit_38->setText(titre);
    if (ui->lineEdit_39)
    {
        const double budget = locale.toDouble(budgetTexte);
        ui->lineEdit_39->setText(QString::number(budget, 'f', 2));
    }
    if (ui->lineEdit_34) ui->lineEdit_34->setText(code);

    if (ui->dateEdit_4)
    {
        QDate date = QDate::fromString(deadlineTexte, Qt::ISODate);
        if (!date.isValid())
        {
            date = QDate::fromString(deadlineTexte, QStringLiteral("dd/MM/yyyy"));
        }
        if (date.isValid())
        {
            ui->dateEdit_4->setDate(date);
        }
    }
    
    // Update QR code
    mettreAJourQRCode();
}

void Gemploye::rafraichirStatistiquesProjet()
{
#ifdef HAVE_CHARTS
    if (!chartViewProjet)
    {
        qDebug() << "Chart view not initialized";
        return;
    }

    QSqlQuery query;
    
    // Improved query - get budget distribution by year with better handling
    const QString requete = QStringLiteral(
        "SELECT "
        "  CASE "
        "    WHEN DEADLINE IS NULL THEN 'No Deadline' "
        "    ELSE TO_CHAR(DEADLINE, 'YYYY') "
        "  END AS annee, "
        "  SUM(BUDGET) AS total_budget, "
        "  COUNT(*) AS project_count "
        "FROM PROJET "
        "GROUP BY CASE "
        "  WHEN DEADLINE IS NULL THEN 'No Deadline' "
        "  ELSE TO_CHAR(DEADLINE, 'YYYY') "
        "END "
        "ORDER BY annee");

    if (!query.exec(requete))
    {
        qDebug() << "Error executing statistics query:" << query.lastError().text();
        chartViewProjet->chart()->removeAllSeries();
        chartViewProjet->chart()->setTitle(tr("Error loading statistics"));
        chartViewProjet->chart()->legend()->hide();
        return;
    }

    // Create pie series with better configuration
    auto *series = new QPieSeries();
    series->setHoleSize(0.35);  // Donut chart
    series->setPieSize(0.80);

    const QLocale locale = QLocale::system();
    QPieSlice* largestSlice = nullptr;
    double maxValue = 0.0;
    int colorIndex = 0;
    
    // Improved color palette - more vibrant and professional
    const QVector<QColor> colorPalette = {
        QColor("#5A6BF2"),  // Blue
        QColor("#FF8C68"),  // Orange
        QColor("#4BC999"),  // Green
        QColor("#FFC107"),  // Yellow
        QColor("#9C6BFF"),  // Purple
        QColor("#1EC4FF"),  // Cyan
        QColor("#FF6B9D"),  // Pink
        QColor("#00BCD4")   // Teal
    };

    bool hasData = false;
    int totalProjects = 0;
    double totalBudget = 0.0;
    
    while (query.next())
    {
        const QString year = query.value(0).toString();
        const double budgetSum = query.value(1).toDouble();
        const int projectCount = query.value(2).toInt();

        // Skip zero budgets
        if (qFuzzyIsNull(budgetSum))
        {
            continue;
        }

        hasData = true;
        totalProjects += projectCount;
        totalBudget += budgetSum;
        
        // Create slice
        auto *slice = series->append(year, budgetSum);
        slice->setBrush(colorPalette.at(colorIndex % colorPalette.size()));
        slice->setLabelVisible(slice->percentage() >= 0.05); // Show label if >= 5%
        
        // Format label with year, budget amount, percentage, and project count
        const QString budgetFormatted = locale.toString(budgetSum, 'f', 0);
        const QString percentageStr = locale.toString(slice->percentage() * 100.0, 'f', 1);
        slice->setLabel(QStringLiteral("%1\n%2 (%3%)\n%4 project(s)")
                        .arg(year.isEmpty() || year == "No Deadline" ? tr("No Deadline") : year)
                        .arg(budgetFormatted)
                        .arg(percentageStr)
                        .arg(projectCount));
        slice->setLabelColor(Qt::black);
        slice->setLabelFont(QFont("Segoe UI", 9));

        // Track largest slice
        if (budgetSum > maxValue)
        {
            maxValue = budgetSum;
            largestSlice = slice;
        }

        ++colorIndex;
    }

    auto *chart = chartViewProjet->chart();
    chart->removeAllSeries();

    if (!hasData)
    {
        delete series;
        chart->setTitle(tr("No project data available"));
        chart->legend()->hide();
        
        qDebug() << "No data found for project statistics";
        return;
    }

    // Explode the largest slice for emphasis
    if (largestSlice)
    {
        largestSlice->setExploded(true);
        largestSlice->setExplodeDistanceFactor(0.08);
        largestSlice->setLabelVisible(true);
        largestSlice->setBorderColor(Qt::white);
        largestSlice->setBorderWidth(2);
    }

    // Configure series labels
    series->setLabelsPosition(QPieSlice::LabelOutside);
    series->setLabelsVisible(true);

    // Add series to chart
    chart->addSeries(series);
    
    // Set chart title with summary
    const QString totalBudgetFormatted = locale.toString(totalBudget, 'f', 0);
    chart->setTitle(QStringLiteral("📊 Budget Distribution by Deadline Year\nTotal: %1 | %2 Projects")
                   .arg(totalBudgetFormatted)
                   .arg(totalProjects));
    
    // Configure chart appearance
    QFont titleFont("Segoe UI", 12, QFont::Bold);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(QColor("#2a174c")));
    
    // Configure legend
    chart->legend()->show();
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor("#2a174c"));
    chart->legend()->setFont(QFont("Segoe UI", 9));
    
    // Add animation
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationDuration(1000);
    
    qDebug() << "Statistics refreshed successfully:" << totalProjects << "projects," << totalBudgetFormatted << "total budget";
    
#else
    qDebug() << "Charts module not available. Statistics will not be displayed.";
    qDebug() << "To enable charts, add QT += charts to your .pro file and rebuild";
#endif
}
void Gemploye::on_Employ_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Gemploye::on_Employ_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    verifierDeadlinesProjets();
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

void Gemploye::on_pushButton_32_clicked()
{
    QString erreur;
    if (!verifierChampsProjet(erreur))
    {
        afficherMessageProjet(erreur, QMessageBox::Warning);
        return;
    }

    const QString code = ui->lineEdit_44->text().trimmed();
    const QString titre = ui->lineEdit_38->text().trimmed();
    const double budget = ui->lineEdit_39->text().trimmed().toDouble();
    const QString deadline = ui->dateEdit_4->date().toString(Qt::ISODate);

    Projet projet(code, titre, budget, deadline);
    Projet existant;
    bool succes = false;

    if (existant.rechercherParCode(code))
    {
        succes = projet.modifier();
        afficherMessageProjet(
            succes ? tr("Projet mis à jour avec succès.")
                   : tr("La mise à jour du projet a échoué."),
            succes ? QMessageBox::Information : QMessageBox::Critical);
    }
    else
    {
        succes = projet.ajouter();
        afficherMessageProjet(
            succes ? tr("Projet ajouté avec succès.")
                   : tr("L'ajout du projet a échoué (code peut-être déjà utilisé)."),
            succes ? QMessageBox::Information : QMessageBox::Critical);
    }

    if (succes)
    {
        chargerTableProjets();
        viderFormulaireProjet();
        verifierDeadlinesProjets();
        mettreAJourQRCode();
    }
}

void Gemploye::on_pushButton_33_clicked()
{
    viderFormulaireProjet();
}

void Gemploye::on_pushButton_13_clicked()
{
    QString code = ui->lineEdit_34->text().trimmed();
    if (code.isEmpty())
    {
        code = ui->lineEdit_44->text().trimmed();
    }

    if (code.isEmpty())
    {
        afficherMessageProjet(tr("Veuillez sélectionner ou saisir le code du projet à supprimer."),
                              QMessageBox::Warning);
        return;
    }

    Projet projet;
    if (projet.supprimer(code))
    {
        afficherMessageProjet(tr("Projet supprimé avec succès."));
        chargerTableProjets();
        viderFormulaireProjet();
        verifierDeadlinesProjets();
    }
    else
    {
        afficherMessageProjet(tr("La suppression du projet a échoué."),
                              QMessageBox::Critical);
    }
}

void Gemploye::on_pushButton_12_clicked()
{
    const QString motCle = ui->lineEdit_16->text().trimmed();
    std::unique_ptr<QSqlQueryModel> model(projetCourant.rechercher(motCle));

    if (!model)
    {
        afficherMessageProjet(tr("Erreur lors de la recherche de projets."),
                              QMessageBox::Critical);
        return;
    }

    chargerTableProjets(model.get());
}

void Gemploye::on_lineEdit_16_textChanged(const QString& text)
{
    const QString motCle = text.trimmed();
    if (motCle.isEmpty())
    {
        chargerTableProjets();
        return;
    }

    std::unique_ptr<QSqlQueryModel> model(projetCourant.rechercher(motCle));
    if (model)
    {
        chargerTableProjets(model.get());
    }
}

void Gemploye::on_pushButton_6_clicked()
{
    if (!ui->comboBox_2)
    {
        return;
    }

    const QString choix = ui->comboBox_2->currentText();
    std::unique_ptr<QSqlQueryModel> model;

    if (choix.contains("Code"))
    {
        model.reset(projetCourant.trierParCode());
    }
    else if (choix.contains("Titre"))
    {
        model.reset(projetCourant.trierParTitre());
    }
    else if (choix.contains("Budget"))
    {
        model.reset(projetCourant.trierParBudget());
    }
    else if (choix.contains("Deadline"))
    {
        model.reset(projetCourant.trierParDeadline());
    }
    else
    {
        model.reset(projetCourant.afficher());
    }

    if (!model)
    {
        afficherMessageProjet(tr("Impossible de trier les projets pour le moment."),
                              QMessageBox::Critical);
        return;
    }

    chargerTableProjets(model.get());
}

void Gemploye::on_tableWidget_2_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    remplirFormulaireDepuisLigne(row);
}

void Gemploye::generateProjectQRCode(int row)
{
    Q_UNUSED(row);
    
    // Get all projects with deadlines within 7 days
    Projet projet;
    QSqlQueryModel* model = projet.afficherProjetsUrgents();
    
    if (!model || model->rowCount() == 0)
    {
        QMessageBox::information(this, tr("QR Code Projects"), 
            tr("No projects with deadlines within the next 7 days."));
        if (model) delete model;
        return;
    }
    
    // Get current date for calculating days remaining
    const QDate today = QDate::currentDate();
    
    // Build simple warning messages for urgent projects
    QString projectData = QString("⚠️ URGENT PROJECT DEADLINES ⚠️\n\n");
    
    for (int i = 0; i < model->rowCount(); ++i)
    {
        QString titre = model->data(model->index(i, 1)).toString();
        QString deadlineStr = model->data(model->index(i, 3)).toString();
        
        // Parse deadline and calculate days remaining
        QDate deadline = QDate::fromString(deadlineStr, "yyyy-MM-dd");
        int daysRemaining = today.daysTo(deadline);
        
        QString message;
        if (daysRemaining < 0)
        {
            message = QString("❌ Project '%1' deadline EXPIRED %2 day(s) ago!\n\n")
                     .arg(titre)
                     .arg(-daysRemaining);
        }
        else if (daysRemaining == 0)
        {
            message = QString("🔴 Project '%1' deadline expires TODAY!\n\n")
                     .arg(titre);
        }
        else if (daysRemaining == 1)
        {
            message = QString("⏰ Project '%1' deadline expires in 1 day!\n\n")
                     .arg(titre);
        }
        else
        {
            message = QString("⏰ Project '%1' deadline expires in %2 days\n\n")
                     .arg(titre)
                     .arg(daysRemaining);
        }
        
        projectData += message;
    }
    
    projectData += QString("\nTotal: %1 urgent project(s)").arg(model->rowCount());
    
    // Generate QR code
    QImage qrImage = qrGenerator->generateQRCode(projectData, 300);
    
    // Create a dialog to display the QR code
    QDialog *qrDialog = new QDialog(this);
    qrDialog->setWindowTitle(tr("Urgent Projects QR Code"));
    qrDialog->setFixedSize(400, 500);
    
    QVBoxLayout *layout = new QVBoxLayout(qrDialog);
    
    // Add info label
    QLabel *infoLabel = new QLabel(QString("<b>⚠️ Deadline Warnings</b><br/>%1 urgent project(s)").arg(model->rowCount()));
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);
    
    // Add QR code image
    QLabel *qrLabel = new QLabel();
    qrLabel->setPixmap(QPixmap::fromImage(qrImage));
    qrLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(qrLabel);
    
    // Add instruction label
    QLabel *instructionLabel = new QLabel(tr("Scan to see which projects are expiring soon"));
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setWordWrap(true);
    layout->addWidget(instructionLabel);
    
    // Add close button
    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, &QPushButton::clicked, qrDialog, &QDialog::close);
    layout->addWidget(closeButton);
    
    qrDialog->setLayout(layout);
    qrDialog->exec();
    
    // Clean up
    delete model;
    qrDialog->deleteLater();
}


void Gemploye::on_generateQRCode_clicked()
{
    // Generate QR code for all projects with deadlines within 7 days
    generateProjectQRCode(0); // row parameter is now unused
}


void Gemploye::on_scanQRCode_clicked()
{
    // Open QR scanner dialog
    QRCodeScanner *scanner = new QRCodeScanner(this);
    scanner->exec();
    scanner->deleteLater();
}

void Gemploye::verifierDeadlinesProjets()
{
    QSqlQuery query;
    const QDate aujourdhui = QDate::currentDate();
    const QDate dans7Jours = aujourdhui.addDays(7);
    
    query.prepare(
        "SELECT COUNT(*) "
        "FROM PROJET "
        "WHERE DEADLINE < TO_DATE(:aujourdhui, 'YYYY-MM-DD') "
        "   OR DEADLINE <= TO_DATE(:dans7jours, 'YYYY-MM-DD')");
    
    query.bindValue(":aujourdhui", aujourdhui.toString(Qt::ISODate));
    query.bindValue(":dans7jours", dans7Jours.toString(Qt::ISODate));
    
    nombreNotifications = 0;
    
    if (query.exec() && query.next())
    {
        nombreNotifications = query.value(0).toInt();
    }
    
    mettreAJourBadgeNotification(nombreNotifications);
    
    // Show Windows system notification if there are urgent projects
    if (nombreNotifications > 0)
    {
        afficherNotificationsWindows();
    }
}

void Gemploye::mettreAJourBadgeNotification(int count)
{
    if (!ui->notificationButton)
    {
        return;
    }

    QString styleSheet = QStringLiteral(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff6b6b, stop:1 #ee5a6f);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 20px;"
        "    padding: 8px 16px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 40px;"
        "    min-height: 40px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff5252, stop:1 #e53935);"
        "    transform: scale(1.05);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #e53935, stop:1 #c62828);"
        "}");

    if (count > 0)
    {
        ui->notificationButton->setText(QStringLiteral("🔔 %1").arg(count));
        ui->notificationButton->setStyleSheet(styleSheet);
        ui->notificationButton->setVisible(true);
    }
    else
    {
        ui->notificationButton->setText(QStringLiteral("🔔"));
        styleSheet.replace("stop:0 #ff6b6b, stop:1 #ee5a6f", "stop:0 #5A6BF2, stop:1 #3452c9");
        styleSheet.replace("stop:0 #ff5252, stop:1 #e53935", "stop:0 #4a5ae8, stop:1 #2a3fc7");
        styleSheet.replace("stop:0 #e53935, stop:1 #c62828", "stop:0 #2a3fc7, stop:1 #1a2fa5");
        ui->notificationButton->setStyleSheet(styleSheet);
    }
}

void Gemploye::afficherNotifications()
{
    QSqlQuery query;
    const QDate aujourdhui = QDate::currentDate();
    const QDate dans7Jours = aujourdhui.addDays(7);
    
    query.prepare(
        "SELECT CODE, TITRE, DEADLINE "
        "FROM PROJET "
        "WHERE DEADLINE < TO_DATE(:aujourdhui, 'YYYY-MM-DD') "
        "   OR DEADLINE <= TO_DATE(:dans7jours, 'YYYY-MM-DD') "
        "ORDER BY DEADLINE ASC");
    
    query.bindValue(":aujourdhui", aujourdhui.toString(Qt::ISODate));
    query.bindValue(":dans7jours", dans7Jours.toString(Qt::ISODate));
    
    if (!ui->notificationPanel)
    {
        return;
    }
    
    // Get the widget inside the scroll area
    QWidget* contentWidget = ui->notificationPanel->widget();
    if (!contentWidget)
    {
        contentWidget = new QWidget();
        contentWidget->setObjectName("scrollAreaWidgetContents");
        contentWidget->setStyleSheet("background-color: #ffffff;");
        ui->notificationPanel->setWidget(contentWidget);
    }
    
    // Ensure content widget has white background
    contentWidget->setStyleSheet("background-color: #ffffff;");

    // Clear existing notifications
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contentWidget->layout());
    if (layout)
    {
    QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr)
    {
        if (item->widget())
        {
            item->widget()->deleteLater();
        }
        delete item;
    }
    }
    else
    {
        layout = new QVBoxLayout(contentWidget);
        layout->setSpacing(8);
        layout->setContentsMargins(8, 8, 8, 8);
        contentWidget->setLayout(layout);
    }

    if (!query.exec())
    {
        QLabel* noNotifications = new QLabel(tr("Aucune notification disponible."), contentWidget);
        noNotifications->setAlignment(Qt::AlignCenter);
        noNotifications->setStyleSheet(QStringLiteral(
            "color: #666;"
            "padding: 15px;"
            "font-size: 13px;"
            "background-color: white;"
            "border: none;"));
        layout->addWidget(noNotifications);
        ui->notificationPanel->setVisible(true);
        return;
    }

    bool hasNotifications = false;
    
    while (query.next())
    {
        hasNotifications = true;
        QString code = query.value(0).toString();
        QString titre = query.value(1).toString();
        QDate deadline = query.value(2).toDate();
        
        int joursRestants = aujourdhui.daysTo(deadline);
        bool isExpired = joursRestants < 0;
        
        QFrame* notificationFrame = new QFrame(contentWidget);
        notificationFrame->setFrameShape(QFrame::StyledPanel);
        notificationFrame->setFrameShadow(QFrame::Raised);
        
        QString bgColor, borderColor, icon;
        QString message;
        
        if (isExpired)
        {
            bgColor = "#ffebee";
            borderColor = "#ef5350";
            icon = "⚠️";
            message = tr("ÉCHÉANCE DÉPASSÉE");
        }
        else
        {
            bgColor = "#fff3e0";
            borderColor = "#ff9800";
            icon = "⏰";
            message = joursRestants == 0 ? tr("ÉCHÉANCE AUJOURD'HUI") : 
                     tr("ÉCHÉANCE DANS %1 JOUR(S)").arg(joursRestants);
        }
        
        notificationFrame->setStyleSheet(QStringLiteral(
            "QFrame {"
            "    background-color: %1;"
            "    border: 2px solid %2;"
            "    border-radius: 8px;"
            "    padding: 8px;"
            "    margin: 2px;"
            "}"
            "QLabel {"
            "    background-color: transparent;"
            "    border: none;"
            "}").arg(bgColor, borderColor));
        
        QHBoxLayout* frameLayout = new QHBoxLayout(notificationFrame);
        frameLayout->setSpacing(8);
        frameLayout->setContentsMargins(4, 4, 4, 4);
        
        QLabel* iconLabel = new QLabel(icon, notificationFrame);
        iconLabel->setStyleSheet(QStringLiteral(
            "font-size: 20px;"
            "background: transparent;"
            "padding: 0px;"));
        frameLayout->addWidget(iconLabel);
        
        QVBoxLayout* textLayout = new QVBoxLayout();
        textLayout->setSpacing(2);
        
        QLabel* titleLabel = new QLabel(titre, notificationFrame);
        titleLabel->setStyleSheet(QStringLiteral(
            "font-weight: bold;"
            "font-size: 13px;"
            "color: #2a174c;"
            "background: transparent;"));
        titleLabel->setWordWrap(true);
        textLayout->addWidget(titleLabel);
        
        QLabel* codeLabel = new QLabel(QStringLiteral("Code: %1").arg(code), notificationFrame);
        codeLabel->setStyleSheet(QStringLiteral(
            "font-size: 11px;"
            "color: #666;"
            "background: transparent;"));
        textLayout->addWidget(codeLabel);
        
        QLabel* deadlineLabel = new QLabel(
            QStringLiteral("%1 - %2").arg(message, deadline.toString("dd/MM/yyyy")), 
            notificationFrame);
        deadlineLabel->setStyleSheet(QStringLiteral(
            "font-size: 11px;"
            "color: %1;"
            "font-weight: bold;"
            "background: transparent;").arg(borderColor));
        deadlineLabel->setWordWrap(true);
        textLayout->addWidget(deadlineLabel);
        
        frameLayout->addLayout(textLayout);
        frameLayout->addStretch();
        
        layout->addWidget(notificationFrame);
    }
    
    if (!hasNotifications)
    {
        QLabel* noNotifications = new QLabel(tr("✅ Tous les projets sont à jour!"), contentWidget);
        noNotifications->setAlignment(Qt::AlignCenter);
        noNotifications->setStyleSheet(QStringLiteral(
            "color: #4caf50;"
            "padding: 20px;"
            "font-size: 14px;"
            "font-weight: bold;"
            "background-color: white;"
            "border: none;"));
        layout->addWidget(noNotifications);
    }
    
    layout->addStretch();
    
    // Ensure the panel is visible and raised
    ui->notificationPanel->setVisible(true);
    ui->notificationPanel->raise();
    contentWidget->updateGeometry();
    ui->notificationPanel->update();
}

void Gemploye::on_notificationButton_clicked()
{
    if (!ui->notificationPanel)
    {
        return;
    }
    
    if (ui->notificationPanel->isVisible())
    {
        ui->notificationPanel->setVisible(false);
    }
    else
    {
        afficherNotifications();
    }
}

void Gemploye::afficherNotificationsWindows()
{
    if (!systemTrayIcon || !QSystemTrayIcon::supportsMessages())
    {
        qDebug() << "System tray notifications not supported on this system";
        return;
    }
    
    QSqlQuery query;
    const QDate aujourdhui = QDate::currentDate();
    const QDate dans7Jours = aujourdhui.addDays(7);
    
    query.prepare(
        "SELECT CODE, TITRE, DEADLINE "
        "FROM PROJET "
        "WHERE DEADLINE < TO_DATE(:aujourdhui, 'YYYY-MM-DD') "
        "   OR DEADLINE <= TO_DATE(:dans7jours, 'YYYY-MM-DD') "
        "ORDER BY DEADLINE ASC");
    
    query.bindValue(":aujourdhui", aujourdhui.toString(Qt::ISODate));
    query.bindValue(":dans7jours", dans7Jours.toString(Qt::ISODate));
    
    if (!query.exec())
    {
        qDebug() << "Error querying urgent projects for notifications";
        return;
    }
    
    // Build notification message
    QString notificationTitle;
    QString notificationMessage;
    QSystemTrayIcon::MessageIcon icon;
    
    int count = 0;
    int expiredCount = 0;
    int todayCount = 0;
    int urgentCount = 0;
    
    // Count different types of deadlines
    while (query.next())
    {
        QDate deadline = query.value(2).toDate();
        int joursRestants = aujourdhui.daysTo(deadline);
        
        count++;
        if (joursRestants < 0)
            expiredCount++;
        else if (joursRestants == 0)
            todayCount++;
        else
            urgentCount++;
    }
    
    // Reset query to get first few projects for the message
    query.exec();
    
    // Determine notification urgency and title
    if (expiredCount > 0)
    {
        icon = QSystemTrayIcon::Critical;
        notificationTitle = tr("⚠️ URGENT: Deadlines Expired!");
    }
    else if (todayCount > 0)
    {
        icon = QSystemTrayIcon::Warning;
        notificationTitle = tr("🔴 Deadlines Due Today!");
    }
    else
    {
        icon = QSystemTrayIcon::Information;
        notificationTitle = tr("⏰ Upcoming Deadlines");
    }
    
    // Build message with summary
    if (expiredCount > 0)
    {
        notificationMessage += tr("❌ %1 project(s) EXPIRED\n").arg(expiredCount);
    }
    if (todayCount > 0)
    {
        notificationMessage += tr("🔴 %1 project(s) due TODAY\n").arg(todayCount);
    }
    if (urgentCount > 0)
    {
        notificationMessage += tr("⏰ %1 project(s) due within 7 days\n").arg(urgentCount);
    }
    
    notificationMessage += tr("\nTotal: %1 urgent project(s)\n\n").arg(count);
    
    // Add details for first 3 urgent projects
    query.exec();
    int displayed = 0;
    while (query.next() && displayed < 3)
    {
        QString titre = query.value(1).toString();
        QDate deadline = query.value(2).toDate();
        int joursRestants = aujourdhui.daysTo(deadline);
        
        if (joursRestants < 0)
        {
            notificationMessage += tr("• %1: EXPIRED %2 day(s) ago\n")
                                  .arg(titre)
                                  .arg(-joursRestants);
        }
        else if (joursRestants == 0)
        {
            notificationMessage += tr("• %1: Due TODAY!\n").arg(titre);
        }
        else if (joursRestants == 1)
        {
            notificationMessage += tr("• %1: Due in 1 day\n").arg(titre);
        }
        else
        {
            notificationMessage += tr("• %1: Due in %2 days\n")
                                  .arg(titre)
                                  .arg(joursRestants);
        }
        
        displayed++;
    }
    
    if (count > 3)
    {
        notificationMessage += tr("\n... and %1 more").arg(count - 3);
    }
    
    // Show Windows notification
    systemTrayIcon->showMessage(
        notificationTitle,
        notificationMessage,
        icon,
        10000  // Display for 10 seconds
    );
}


void Gemploye::mettreAJourQRCode()
{
    if (!ui->qrCodeLabel || !qrGenerator)
    {
        return;
    }

    // Get project data from form
    QString titre = ui->lineEdit_38 ? ui->lineEdit_38->text().trimmed() : QString();
    QString deadlineStr = ui->dateEdit_4 ? ui->dateEdit_4->date().toString("yyyy-MM-dd") : QString();
    
    // If essential fields are empty, show placeholder
    if (titre.isEmpty() || deadlineStr.isEmpty())
    {
        ui->qrCodeLabel->setText(tr("Remplissez les champs"));
        ui->qrCodeLabel->setPixmap(QPixmap());
        return;
    }
    
    // Calculate days remaining
    QDate deadline = QDate::fromString(deadlineStr, "yyyy-MM-dd");
    QDate today = QDate::currentDate();
    int daysRemaining = today.daysTo(deadline);
    
    // Create simple warning message
    QString projectData;
    
    if (daysRemaining < 0)
    {
        projectData = QString("❌ Project '%1'\nDeadline EXPIRED %2 day(s) ago!")
                     .arg(titre)
                     .arg(-daysRemaining);
    }
    else if (daysRemaining == 0)
    {
        projectData = QString("🔴 Project '%1'\nDeadline expires TODAY!")
                     .arg(titre);
    }
    else if (daysRemaining == 1)
    {
        projectData = QString("⏰ Project '%1'\nDeadline expires in 1 day!")
                     .arg(titre);
    }
    else if (daysRemaining <= 7)
    {
        projectData = QString("⏰ Project '%1'\nDeadline expires in %2 days")
                     .arg(titre)
                     .arg(daysRemaining);
    }
    else
    {
        // For projects with deadline > 7 days, show a different message
        projectData = QString("✅ Project '%1'\nDeadline: %2 days remaining")
                     .arg(titre)
                     .arg(daysRemaining);
    }
    
    // Generate QR code image
    QImage qrImage = qrGenerator->generateQRCode(projectData, 200);
    
    // Display QR code
    QPixmap qrPixmap = QPixmap::fromImage(qrImage);
    ui->qrCodeLabel->setPixmap(qrPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->qrCodeLabel->setText("");
}

bool Gemploye::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->label_7)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                on_logo_clicked();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void Gemploye::on_logo_clicked()
{
    // Navigate to home page (Employé page)
    ui->stackedWidget->setCurrentIndex(0);
}

void Gemploye::on_connectArduino_clicked()
{
    if (!arduino)
    {
        arduino = new Arduino();
    }
    
    int result = arduino->connect_arduino();
    
    if (result == 0)
    {
        QMessageBox::information(this, tr("Arduino"), 
            tr("Arduino connecté avec succès sur le port: %1").arg(arduino->getarduino_port_name()));
        
        // Setup timer to read data from Arduino periodically
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Gemploye::readArduinoData);
        timer->start(100); // Read every 100ms
    }
    else if (result == -1)
    {
        QMessageBox::critical(this, tr("Arduino"), 
            tr("Arduino non détecté. Vérifiez que l'Arduino est bien connecté."));
    }
    else
    {
        QMessageBox::critical(this, tr("Arduino"), 
            tr("Impossible d'ouvrir le port Arduino."));
    }
}

void Gemploye::on_sendToArduino_clicked()
{
#ifdef HAVE_SERIALPORT
    if (!arduino || !arduino->getserial()->isOpen())
    {
        QMessageBox::warning(this, tr("Arduino"), 
            tr("Arduino non connecté. Veuillez d'abord connecter l'Arduino."));
        return;
    }
    
    // Example: Send "1" to turn on LED, "0" to turn off
    // You can modify this to send data from UI elements
    QString dataToSend = "1"; // Replace with actual data from your UI
    int result = arduino->write_to_arduino(dataToSend.toUtf8());
    
    if (result == 0)
    {
        qDebug() << "Données envoyées à Arduino: " << dataToSend;
    }
    else
    {
        QMessageBox::warning(this, tr("Arduino"), 
            tr("Erreur lors de l'envoi des données."));
    }
#else
    QMessageBox::warning(this, tr("Arduino"), 
        tr("Support Arduino non disponible. Module Qt SerialPort requis."));
#endif
}

void Gemploye::readArduinoData()
{
#ifdef HAVE_SERIALPORT
    if (!arduino || !arduino->getserial()->isOpen())
    {
        return;
    }
    
    QByteArray data = arduino->read_from_arduino();
    
    if (!data.isEmpty())
    {
        QString receivedData = QString::fromUtf8(data).trimmed();
        qDebug() << "Données reçues d'Arduino: " << receivedData;
        
        // Process the received data here
        // Example: Update UI elements, trigger actions, etc.
    }
#endif
}

void Gemploye::testArduinoConnection()
{
#ifdef HAVE_SERIALPORT
    qDebug() << "Testing Arduino connection...";
    
    if (!arduino)
    {
        arduino = new Arduino();
    }
    
    int result = arduino->connect_arduino();
    
    if (result == 0)
    {
        qDebug() << "✓ Arduino connected successfully on port:" << arduino->getarduino_port_name();
        qDebug() << "Arduino is ready to use!";
        
        // Optional: Show success notification in console
        // You can uncomment this to show a message box:
        // QMessageBox::information(this, tr("Arduino"), 
        //     tr("Arduino connecté avec succès sur le port: %1").arg(arduino->getarduino_port_name()));
        
        // Setup timer to read data from Arduino periodically
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Gemploye::readArduinoData);
        timer->start(100); // Read every 100ms
    }
    else if (result == -1)
    {
        qDebug() << "✗ Arduino not detected. Please check if Arduino is connected.";
        qDebug() << "Make sure Arduino Uno is connected via USB.";
    }
    else
    {
        qDebug() << "✗ Failed to open Arduino port.";
    }
#else
    qDebug() << "⚠ Arduino support is not available.";
    qDebug() << "Qt SerialPort module is required. Please install it via Qt Maintenance Tool.";
#endif
}

void Gemploye::on_pushButton_23_clicked()
{
    exporterProjetsPDF();
}

void Gemploye::exporterProjetsPDF()
{
    // Ask user for file location
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Exporter en PDF"), "",
        tr("Fichiers PDF (*.pdf);;Tous les fichiers (*)"));
    
    if (fileName.isEmpty())
    {
        return;
    }
    
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
    {
        fileName += ".pdf";
    }
    
    // Create PDF printer
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    QPageSize pageSize(QPageSize::A4);
    printer.setPageSize(pageSize);
    QMarginsF margins(15, 15, 15, 15);
    printer.setPageMargins(margins, QPageLayout::Millimeter);
    
    // Use QTextDocument for better rendering
    QTextDocument document;
    document.setPageSize(printer.pageLayout().paintRectPixels(printer.resolution()).size());
    
    // Build HTML content
    QString html;
    const QLocale locale = QLocale::system();
    
    // HTML Header with styles - Larger fonts for better readability
    html += "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; color: #000000; font-size: 14px; }";
    html += "h1 { color: #2a174c; font-size: 32px; font-weight: bold; text-align: center; margin: 30px 0; }";
    html += "h2 { color: #3452c9; font-size: 20px; font-weight: bold; margin-top: 40px; margin-bottom: 15px; }";
    html += "p { color: #000000; font-size: 14px; line-height: 1.8; margin: 8px 0; }";
    html += "table { width: 100%; border-collapse: collapse; margin: 25px 0; font-size: 14px; }";
    html += "th { background-color: #5A6BF2; color: #ffffff; font-weight: bold; padding: 15px; text-align: center; border: 2px solid #3452c9; font-size: 16px; }";
    html += "td { padding: 12px; border: 1px solid #cccccc; text-align: left; font-size: 14px; }";
    html += "tr:nth-child(even) { background-color: #f5f5f5; }";
    html += "tr:nth-child(odd) { background-color: #ffffff; }";
    html += ".stats { background-color: #f0f5ff; padding: 20px; border-left: 5px solid #3452c9; margin: 25px 0; font-size: 14px; }";
    html += ".footer { text-align: center; color: #666666; font-size: 12px; margin-top: 40px; }";
    html += "strong { font-size: 15px; }";
    html += "</style></head><body>";
    
    // Title
    html += "<h1>" + tr("RAPPORT DES PROJETS") + "</h1>";
    html += "<p style='text-align: right; color: #666666;'>" + 
            tr("Généré le: %1").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy à HH:mm")) + "</p>";
    
    // Get statistics
    QSqlQuery statsQuery;
    statsQuery.prepare(
        "SELECT "
        "COUNT(*) AS total_projets, "
        "SUM(BUDGET) AS budget_total, "
        "AVG(BUDGET) AS budget_moyen, "
        "MIN(DEADLINE) AS deadline_min, "
        "MAX(DEADLINE) AS deadline_max "
        "FROM PROJET");
    
    double totalBudget = 0.0;
    double avgBudget = 0.0;
    int totalProjects = 0;
    QString minDeadline, maxDeadline;
    
    if (statsQuery.exec() && statsQuery.next())
    {
        totalProjects = statsQuery.value(0).toInt();
        totalBudget = statsQuery.value(1).toDouble();
        avgBudget = statsQuery.value(2).toDouble();
        QDate minDate = statsQuery.value(3).toDate();
        QDate maxDate = statsQuery.value(4).toDate();
        if (minDate.isValid())
            minDeadline = minDate.toString("dd/MM/yyyy");
        if (maxDate.isValid())
            maxDeadline = maxDate.toString("dd/MM/yyyy");
    }
    
    // Statistics section
    html += "<h2>" + tr("Statistiques") + "</h2>";
    html += "<div class='stats'>";
    html += "<p><strong>" + tr("Nombre total de projets:") + "</strong> " + QString::number(totalProjects) + "</p>";
    html += "<p><strong>" + tr("Budget total:") + "</strong> " + locale.toString(totalBudget, 'f', 2) + " " + tr("DT") + "</p>";
    html += "<p><strong>" + tr("Budget moyen:") + "</strong> " + locale.toString(avgBudget, 'f', 2) + " " + tr("DT") + "</p>";
    html += "<p><strong>" + tr("Échéance la plus proche:") + "</strong> " + 
            (minDeadline.isEmpty() ? tr("N/A") : minDeadline) + "</p>";
    html += "<p><strong>" + tr("Échéance la plus lointaine:") + "</strong> " + 
            (maxDeadline.isEmpty() ? tr("N/A") : maxDeadline) + "</p>";
    html += "</div>";
    
    // Get project data
    QSqlQuery query;
    query.prepare("SELECT CODE, TITRE, BUDGET, DEADLINE FROM PROJET ORDER BY ID_PROJET DESC");
    
    if (!query.exec())
    {
        afficherMessageProjet(tr("Erreur lors de la récupération des données."), QMessageBox::Critical);
        return;
    }
    
    // Projects table
    html += "<h2>" + tr("Liste des Projets") + "</h2>";
    html += "<table>";
    html += "<thead><tr>";
    html += "<th>" + tr("Code") + "</th>";
    html += "<th>" + tr("Titre") + "</th>";
    html += "<th>" + tr("Budget") + "</th>";
    html += "<th>" + tr("Deadline") + "</th>";
    html += "</tr></thead>";
    html += "<tbody>";
    
    int rowNum = 0;
    while (query.next())
    {
        QString code = query.value(0).toString();
        QString titre = query.value(1).toString();
        double budget = query.value(2).toDouble();
        QDate deadline = query.value(3).toDate();
        QString deadlineStr = deadline.isValid() ? deadline.toString("dd/MM/yyyy") : tr("N/A");
        
        html += "<tr>";
        html += "<td style='text-align: center;'><strong>" + code + "</strong></td>";
        html += "<td>" + titre + "</td>";
        html += "<td style='text-align: right;'>" + locale.toString(budget, 'f', 2) + " " + tr("DT") + "</td>";
        html += "<td style='text-align: center;'>" + deadlineStr + "</td>";
        html += "</tr>";
        rowNum++;
    }
    
    html += "</tbody></table>";
    
    // Footer
    html += "<div class='footer'>";
    html += tr("Généré par Gemploye - %1 projets au total").arg(totalProjects);
    html += "</div>";
    
    html += "</body></html>";
    
    // Set HTML content to document
    document.setHtml(html);
    
    // Print to PDF
    document.print(&printer);
    
    afficherMessageProjet(tr("PDF exporté avec succès vers:\n%1").arg(fileName), QMessageBox::Information);
}
