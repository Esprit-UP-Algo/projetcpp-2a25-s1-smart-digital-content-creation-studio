#include "gemploye.h"
#include "ui_gemploye.h"

Gemploye::Gemploye(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gemploye)
{
    ui->setupUi(this);

    QPixmap logo("C:/Users/LOQ/Documents/Gemploye/images/logo.png");
    ui->label_7->setPixmap(logo.scaled(ui->label_7->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

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

