#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSslSocket>
#include <QInputDialog>
#include <QRandomGenerator>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_btnLogin_clicked()
{
    const QString email = ui->lineEmail->text().trimmed();
    const QString password = ui->linePassword->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, tr("Connexion"), tr("Veuillez saisir l'email et le mot de passe."));
        return;
    }

    if (authenticate(email, password)) {
        accept();
    } else {
        QMessageBox::warning(this, tr("Connexion"), tr("Identifiants incorrects."));
    }
}

void LoginDialog::on_btnCancel_clicked()
{
    reject();
}

void LoginDialog::on_btnForgot_clicked()
{
    const QString email = ui->lineEmail->text().trimmed();
    if (email.isEmpty()) {
        QMessageBox::information(this, tr("Mot de passe oublié"), tr("Veuillez d'abord saisir votre email dans le champ identifiant."));
        return;
    }

    sendPasswordByEmail(email);
}

bool LoginDialog::authenticate(const QString &email, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM EMPLOYE WHERE EMAIL = :email AND MDP = :mdp");
    query.bindValue(":email", email);
    query.bindValue(":mdp", password);

    if (!query.exec()) {
        qDebug() << "Erreur SQL authenticate():" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

void LoginDialog::sendPasswordByEmail(const QString &email)
{
    QSqlQuery query;
    query.prepare("SELECT MDP FROM EMPLOYE WHERE EMAIL = :email");
    query.bindValue(":email", email);

    if (!query.exec()) {
        qDebug() << "Erreur SQL sendPasswordByEmail():" << query.lastError().text();
        QMessageBox::critical(this, tr("Mot de passe oublié"), tr("Erreur base de données."));
        return;
    }

    if (!query.next()) {
        QMessageBox::warning(this, tr("Mot de passe oublié"), tr("Aucun compte trouvé pour cet email."));
        return;
    }

    // On ne renvoie plus le mot de passe, on génère un code de réinitialisation
    const QString password = query.value(0).toString(); // on pourrait ne plus l'utiliser, mais on le garde si besoin

    // Génération d'un code à 6 chiffres
    m_resetCode = QString::number(QRandomGenerator::global()->bounded(100000, 999999));
    m_resetEmail = email;
    
    // Paramètres SMTP Gmail - A REMPLACER par ton compte et ton mot de passe d'application
    const QString smtpHost = "smtp.gmail.com";
    const int smtpPort = 465; // SSL
    const QString smtpUser = "hazem.marzougui@esprit.tn";          // TODO: mettre votre adresse Gmail
    const QString smtpPassword = "hxqmsnglmnygqyer";       // TODO: mettre votre mot de passe d'application

    QSslSocket socket;
    socket.connectToHostEncrypted(smtpHost, smtpPort);
    if (!socket.waitForEncrypted(10000)) {
        qDebug() << "Erreur SMTP: connexion SSL échouée" << socket.errorString();
        QMessageBox::critical(this, tr("Mot de passe oublié"), tr("Impossible de se connecter au serveur SMTP."));
        return;
    }

    auto sendLine = [&socket](const QString &line) {
        socket.write(line.toUtf8() + "\r\n");
        socket.flush();
        socket.waitForReadyRead(10000);
        QByteArray resp = socket.readAll();
        qDebug() << "SMTP>>" << resp;
    };

    socket.waitForReadyRead(10000); // bannière
    socket.readAll();

    sendLine("EHLO localhost");

    // Authentification LOGIN
    sendLine("AUTH LOGIN");
    sendLine(smtpUser.toUtf8().toBase64());
    sendLine(smtpPassword.toUtf8().toBase64());

    // Construction du message
    const QString from = smtpUser;
    const QString to = email;
    const QString subject = "Code de reinitialisation de mot de passe";
    const QString body = tr("Bonjour,\n\nVotre code de reinitialisation est : %1\n\nEntrez ce code dans l'application pour choisir un nouveau mot de passe.\n\nCeci est un message automatique.").arg(m_resetCode);

    sendLine("MAIL FROM:<" + from + ">");
    sendLine("RCPT TO:<" + to + ">");
    sendLine("DATA");

    QString data;
    data += "From: <" + from + ">\r\n";
    data += "To: <" + to + ">\r\n";
    data += "Subject: " + subject + "\r\n";
    data += "Content-Type: text/plain; charset=utf-8\r\n";
    data += "\r\n";
    data += body + "\r\n";
    data += "."; // fin du message

    sendLine(data);
    sendLine("QUIT");

    QMessageBox::information(this, tr("Mot de passe oublié"),
                             tr("Un email contenant un code de reinitialisation a été envoyé (si l'adresse existe)."));

    // Après l'envoi, on demande le code et le nouveau mot de passe
    verifyCodeAndChangePassword();
}

bool LoginDialog::verifyCodeAndChangePassword()
{
    if (m_resetEmail.isEmpty() || m_resetCode.isEmpty()) {
        return false;
    }

    bool ok = false;
    QString codeSaisi = QInputDialog::getText(this,
                                              tr("Verification du code"),
                                              tr("Entrez le code recu par email :"),
                                              QLineEdit::Normal,
                                              QString(),
                                              &ok);
    if (!ok || codeSaisi.isEmpty()) {
        return false; // annulé
    }

    if (codeSaisi.trimmed() != m_resetCode) {
        QMessageBox::warning(this, tr("Verification du code"), tr("Code incorrect."));
        return false;
    }

    // Code correct -> demander un nouveau mot de passe
    QString nouveauMdp = QInputDialog::getText(this,
                                               tr("Nouveau mot de passe"),
                                               tr("Entrez le nouveau mot de passe :"),
                                               QLineEdit::Password,
                                               QString(),
                                               &ok);
    if (!ok || nouveauMdp.isEmpty()) {
        return false;
    }

    // Mettre à jour le mot de passe dans la base
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE EMPLOYE SET MDP = :mdp WHERE EMAIL = :email");
    updateQuery.bindValue(":mdp", nouveauMdp);
    updateQuery.bindValue(":email", m_resetEmail);

    if (!updateQuery.exec()) {
        qDebug() << "Erreur SQL verifyCodeAndChangePassword():" << updateQuery.lastError().text();
        QMessageBox::critical(this, tr("Nouveau mot de passe"), tr("Erreur lors de la mise a jour du mot de passe."));
        return false;
    }

    QMessageBox::information(this, tr("Nouveau mot de passe"), tr("Votre mot de passe a ete mis a jour."));
    return true;
}
