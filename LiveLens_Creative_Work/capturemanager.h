#pragma once
#include <QObject>
#include <QPixmap>
#include <QPoint>

class CaptureManager : public QObject
{
    Q_OBJECT
public:
    explicit CaptureManager(QObject *parent = nullptr);
    QPixmap captureRegion();       // показывает селектор области, возвращает скриншот
    QPixmap captureAtCursor();     // захват области вокруг курсора
};