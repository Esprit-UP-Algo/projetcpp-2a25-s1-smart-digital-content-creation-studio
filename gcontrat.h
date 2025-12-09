#ifndef GCONTRAT_H
#define GCONTRAT_H

#include <QMainWindow>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidget>
#include "contrat.h"

#include <QTimer>
#include <QSystemTrayIcon>

#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QBuffer>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui {
class Gcontrat;
}
QT_END_NAMESPACE

class Gcontrat : public QMainWindow
{
    Q_OBJECT

public:
    Gcontrat(QWidget *parent = nullptr);
    ~Gcontrat();
    void setEmbedded(bool embedded = true);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPushButton *chatbotFloatingButton;
    QWidget *chatbotWindow;
    QTextEdit *textEdit_chatbot;
    QLineEdit *lineEdit_chatbot;
    QPushButton *pushButton_chatbot;
    bool chatbotVisible = false;

private slots:
    // Slots pour la navigation
    void on_Employ_clicked();
    void on_Employ_4_clicked();
    void on_Employ_2_clicked();
    void on_Employ_6_clicked();
    void on_Employ_3_clicked();
    void on_Employ_5_clicked();

    //Slots pour la gestion des contrats
    void on_pushButton_10_clicked(); // Ajouter
    void on_pushButton_11_clicked(); // Modifier
    void on_pushButton_7_clicked();  // Supprimer
    void on_pushButton_14_clicked(); // Annuler
    void on_tableWidget_3_cellClicked(int row);
    void on_pushButton_1_clicked(); // Recherche par type kenet _15_
    void on_comboBox_3_currentIndexChanged(const QString &text);
    void on_tabWidget_3_currentChanged(int index);

    void toggleChatbot();
    void on_pushButton_chatbot_clicked();

private:
    Ui::Gcontrat *ui;
    bool m_embedded = false;
    void clearForm();
    void afficherContrats();
    bool sponsorExists(int id);
    bool createurExists(int id);
    void rechercherContratParType();
    void trierContratsParPrix();
    void exporterPDF();
    int selectedId = -1; // Stocke ID_CONTRAT sélectionné

    void updateAlertIcon(bool hasAlert);
    void showAlertContracts();
    void checkContractAlerts();

    void createFloatingChatbot();
    void processChatbotMessage(const QString& message);
    void addChatMessage(const QString& sender, const QString& message, bool isBot = false);

    // Mise en page lorsqu'on intègre Gcontrat dans une autre fenêtre (sans sa barre latérale)
    void applyEmbeddedLayout();

private:
    QSystemTrayIcon *trayIcon;
    QTimer *alertTimer;
};

#endif // GCONTRAT_H
