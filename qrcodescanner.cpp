#include "qrcodescanner.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

QRCodeScanner::QRCodeScanner(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("QR Code Scanner"));
    setFixedSize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *label = new QLabel(tr("QR Code Scanner\n\nThis feature is not yet implemented."), this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    
    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeButton);
}

QRCodeScanner::~QRCodeScanner()
{
}

