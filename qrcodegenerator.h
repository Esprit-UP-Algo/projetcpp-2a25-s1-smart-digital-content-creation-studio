#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H

#include <QObject>
#include <QImage>
#include <QString>

class QRCodeGenerator : public QObject
{
    Q_OBJECT

public:
    explicit QRCodeGenerator(QObject *parent = nullptr);
    QImage generateQRCode(const QString &data, int size = 200);
    QImage generateQRCode(const QString &data, int size, int margin);

private:
    QImage createQRImage(const QString &data, int size, int margin);
    void drawFinderPattern(QPainter *painter, int x, int y, int cellSize);
    QImage generateQRCodeFromAPI(const QString &data, int size);
};

#endif // QRCODEGENERATOR_H

