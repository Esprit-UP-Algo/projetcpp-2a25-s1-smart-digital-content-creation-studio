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
#include <QTableWidgetItem>
#include <QStringList>
#include <QVector>
#include <QVBoxLayout>
#include <QPainter>
#include <QMargins>
#include <QBrush>
#include <memory>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLegend>

Gemploye::Gemploye(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gemploye)
{
    ui->setupUi(this);

    QPixmap logo(":/images/logo.png");
    ui->label_7->setPixmap(logo.scaled(ui->label_7->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

Gemploye::~Gemploye()
{
    delete ui;
}

void Gemploye::initAfterConnect()
{
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

    initialiserProjetUi();
    rafraichirStatistiquesProjet();
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
}

void Gemploye::viderFormulaireProjet()
{
    if (ui->lineEdit_44) ui->lineEdit_44->clear();
    if (ui->lineEdit_38) ui->lineEdit_38->clear();
    if (ui->lineEdit_39) ui->lineEdit_39->clear();
    if (ui->lineEdit_34) ui->lineEdit_34->clear();
    if (ui->dateEdit_4) ui->dateEdit_4->setDate(QDate::currentDate());
    if (ui->tableWidget_2) ui->tableWidget_2->clearSelection();
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
}

void Gemploye::rafraichirStatistiquesProjet()
{
    if (!chartViewProjet)
    {
        return;
    }

    QSqlQuery query;
    const QString requete = QStringLiteral(
        "SELECT NVL(TO_CHAR(DEADLINE, 'YYYY'), 'Sans échéance') AS annee, "
        "       SUM(BUDGET) AS total "
        "FROM PROJET "
        "GROUP BY NVL(TO_CHAR(DEADLINE, 'YYYY'), 'Sans échéance') "
        "ORDER BY annee");

    if (!query.exec(requete))
    {
        chartViewProjet->chart()->removeAllSeries();
        chartViewProjet->chart()->setTitle(tr("Statistiques indisponibles"));
        return;
    }

    auto *series = new QPieSeries();
    series->setHoleSize(0.35);
    series->setPieSize(0.80);

    const QLocale locale = QLocale::system();
    QPieSlice* meilleureSlice = nullptr;
    double maxValue = 0.0;
    int couleurIndex = 0;
    const QVector<QColor> paletteCouleurs = {
        QColor("#5A6BF2"),
        QColor("#FF8C68"),
        QColor("#4BC999"),
        QColor("#FFC107"),
        QColor("#9C6BFF"),
        QColor("#1EC4FF")
    };

    bool hasData = false;
    while (query.next())
    {
        const QString annee = query.value(0).toString();
        const double total = query.value(1).toDouble();

        if (qFuzzyIsNull(total))
        {
            continue;
        }

        hasData = true;
        auto *slice = series->append(annee, total);
        slice->setBrush(paletteCouleurs.at(couleurIndex % paletteCouleurs.size()));
        slice->setLabelVisible(slice->percentage() >= 0.08);
        const QString montant = locale.toString(total, 'f', 0);
        slice->setLabel(QStringLiteral("%1 • %2 (%3%)")
                        .arg(annee.isEmpty() ? tr("Sans échéance") : annee,
                             montant,
                             locale.toString(slice->percentage() * 100.0, 'f', 1)));
        slice->setLabelColor(Qt::black);

        if (total > maxValue)
        {
            maxValue = total;
            meilleureSlice = slice;
        }

        ++couleurIndex;
    }

    auto *chart = chartViewProjet->chart();
    chart->removeAllSeries();

    if (!hasData)
    {
        delete series;
        chart->setTitle(tr("Aucune donnée projet disponible"));
        chart->legend()->hide();
        return;
    }

    if (meilleureSlice)
    {
        meilleureSlice->setExploded(true);
        meilleureSlice->setExplodeDistanceFactor(0.05);
        meilleureSlice->setLabelVisible(true);
    }

    series->setLabelsPosition(QPieSlice::LabelOutside);

    chart->addSeries(series);
    chart->setTitle(tr("Répartition des budgets par année d'échéance"));
    chart->legend()->show();
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
