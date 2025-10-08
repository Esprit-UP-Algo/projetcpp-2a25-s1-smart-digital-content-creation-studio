#include "gsponsors.h"
#include "ui_gsponsors.h"

Gsponsors::Gsponsors(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gsponsors)
{
    ui->setupUi(this);
    ui->image1->setPixmap(QPixmap("C:/Users/walaa/OneDrive - ESPRIT/Documents/Gsponsors/images/logo.png"));
    ui->image1->setScaledContents(true);

    ui->image3->setPixmap(QPixmap("C:/Users/walaa/OneDrive - ESPRIT/Documents/Gsponsors/images/logo.png"));
    ui->image3->setScaledContents(true);
}

Gsponsors::~Gsponsors()
{
    delete ui;
}
