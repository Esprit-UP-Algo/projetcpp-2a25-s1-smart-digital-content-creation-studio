#ifndef QRCODESCANNER_H
#define QRCODESCANNER_H

#include <QDialog>

class QRCodeScanner : public QDialog
{
    Q_OBJECT

public:
    explicit QRCodeScanner(QWidget *parent = nullptr);
    ~QRCodeScanner();
};

#endif // QRCODESCANNER_H

