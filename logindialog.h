#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_btnLogin_clicked();
    void on_btnCancel_clicked();
    void on_btnForgot_clicked();

private:
    Ui::LoginDialog *ui;

    bool authenticate(const QString &email, const QString &password);
    void sendPasswordByEmail(const QString &email);

    // Gestion de la réinitialisation par code
    QString m_resetEmail;
    QString m_resetCode;
    bool verifyCodeAndChangePassword();
};

#endif // LOGINDIALOG_H
