/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *labelLogo;
    QWidget *rightWidget;
    QVBoxLayout *verticalLayout;
    QLabel *labelIdentifiant;
    QLineEdit *lineEmail;
    QLabel *labelMdp;
    QLineEdit *linePassword;
    QHBoxLayout *horizontalLayoutForgot;
    QPushButton *btnForgot;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayoutButtons;
    QSpacerItem *horizontalSpacer2;
    QPushButton *btnCancel;
    QPushButton *btnLogin;
    QPushButton *btnFaceID;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(600, 350);
        LoginDialog->setStyleSheet(QString::fromUtf8("QDialog {\n"
"    background-color: qlineargradient(\n"
"        x1:0, y1:0, x2:1, y2:1,\n"
"        stop:0 #2a174c,\n"
"        stop:0.5 #3452c9,\n"
"        stop:1 #c7386d\n"
"    );\n"
"}\n"
"\n"
"QLabel {\n"
"    color: white;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QLineEdit {\n"
"    background-color: rgba(255, 255, 255, 30);\n"
"    color: white;\n"
"    border: 1px solid white;\n"
"    border-radius: 8px;\n"
"    padding: 5px;\n"
"}\n"
"\n"
"QPushButton {\n"
"    background-color: qlineargradient(\n"
"        x1: 0, y1: 0, x2: 0, y2: 1,\n"
"        stop: 0 #ff8c00,\n"
"        stop: 1 #ffa500\n"
"    );\n"
"    color: white;\n"
"    border: 2px solid transparent;\n"
"    border-radius: 8px;\n"
"    padding: 6px 16px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    border: 2px solid #ffcc00;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgb(0, 170, 127);\n"
"}\n"
"\n"
"QPushButton#btnForgot {\n"
"    background-color: transparent;\n"
"    color: #ffcc00;\n"
"    "
                        "border: none;\n"
"    text-decoration: underline;\n"
"}\n"
"\n"
"QPushButton#btnForgot:hover {\n"
"    color: #ffffff;\n"
"}\n"
"\n"
"QPushButton#btnFaceID {\n"
"    background-color: qlineargradient(\n"
"        x1: 0, y1: 0, x2: 0, y2: 1,\n"
"        stop: 0 #4CAF50,\n"
"        stop: 1 #45a049\n"
"    );\n"
"    color: white;\n"
"    border: 2px solid transparent;\n"
"    border-radius: 8px;\n"
"    padding: 6px 16px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton#btnFaceID:hover {\n"
"    border: 2px solid #66ff66;\n"
"}\n"
""));
        horizontalLayout = new QHBoxLayout(LoginDialog);
        horizontalLayout->setObjectName("horizontalLayout");
        labelLogo = new QLabel(LoginDialog);
        labelLogo->setObjectName("labelLogo");
        labelLogo->setMinimumSize(QSize(250, 250));
        labelLogo->setStyleSheet(QString::fromUtf8("image: url(:/images/logo.png);\n"
"background-color: transparent;\n"
"background-position: center;\n"
"background-repeat: no-repeat;"));
        labelLogo->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(labelLogo);

        rightWidget = new QWidget(LoginDialog);
        rightWidget->setObjectName("rightWidget");
        verticalLayout = new QVBoxLayout(rightWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        labelIdentifiant = new QLabel(rightWidget);
        labelIdentifiant->setObjectName("labelIdentifiant");

        verticalLayout->addWidget(labelIdentifiant);

        lineEmail = new QLineEdit(rightWidget);
        lineEmail->setObjectName("lineEmail");

        verticalLayout->addWidget(lineEmail);

        labelMdp = new QLabel(rightWidget);
        labelMdp->setObjectName("labelMdp");

        verticalLayout->addWidget(labelMdp);

        linePassword = new QLineEdit(rightWidget);
        linePassword->setObjectName("linePassword");
        linePassword->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(linePassword);

        horizontalLayoutForgot = new QHBoxLayout();
        horizontalLayoutForgot->setObjectName("horizontalLayoutForgot");
        btnForgot = new QPushButton(rightWidget);
        btnForgot->setObjectName("btnForgot");
        btnForgot->setFlat(true);

        horizontalLayoutForgot->addWidget(btnForgot);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayoutForgot->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayoutForgot);

        horizontalLayoutButtons = new QHBoxLayout();
        horizontalLayoutButtons->setObjectName("horizontalLayoutButtons");
        horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayoutButtons->addItem(horizontalSpacer2);

        btnCancel = new QPushButton(rightWidget);
        btnCancel->setObjectName("btnCancel");

        horizontalLayoutButtons->addWidget(btnCancel);

        btnLogin = new QPushButton(rightWidget);
        btnLogin->setObjectName("btnLogin");

        horizontalLayoutButtons->addWidget(btnLogin);


        verticalLayout->addLayout(horizontalLayoutButtons);

        btnFaceID = new QPushButton(rightWidget);
        btnFaceID->setObjectName("btnFaceID");

        verticalLayout->addWidget(btnFaceID);


        horizontalLayout->addWidget(rightWidget);


        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Connexion", nullptr));
        labelLogo->setText(QString());
        labelIdentifiant->setText(QCoreApplication::translate("LoginDialog", "email", nullptr));
        labelMdp->setText(QCoreApplication::translate("LoginDialog", "mot de passe", nullptr));
        btnForgot->setText(QCoreApplication::translate("LoginDialog", "Mot de passe oubli\303\251 ?", nullptr));
        btnCancel->setText(QCoreApplication::translate("LoginDialog", "Annuler", nullptr));
        btnLogin->setText(QCoreApplication::translate("LoginDialog", "Se connecter", nullptr));
        btnFaceID->setText(QCoreApplication::translate("LoginDialog", "\360\237\223\270 Face ID", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
