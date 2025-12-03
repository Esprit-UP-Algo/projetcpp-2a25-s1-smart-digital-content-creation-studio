#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    
    QString getUserRole() const { return m_userRole; }

private slots:
    void on_btnLogin_clicked();
    void on_btnCancel_clicked();
    void on_btnForgot_clicked();
    void on_btnFaceID_clicked();

private:
    Ui::LoginDialog *ui;

    bool authenticate(const QString &email, const QString &password);
    void sendPasswordByEmail(const QString &email);
    void authenticateWithFaceID();

    // Gestion de la réinitialisation par code
    QString m_resetEmail;
    QString m_resetCode;
    QString m_userRole; // Stocke le rôle de l'utilisateur connecté
    bool verifyCodeAndChangePassword();
};

#endif // LOGINDIALOG_H
