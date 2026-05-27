#include "capturemanager.h"
#include "regionselector.h"
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <QPixmap>
//захват экрана, выбирает пользователь
CaptureManager::CaptureManager(QObject *parent) : QObject(parent) {}

QPixmap CaptureManager::captureRegion()//возвращает снимок выбранной области
{
    RegionSelector selector;
    QRect rect = selector.getSelection();
    if (rect.isNull()) return QPixmap();
    QScreen *screen = QGuiApplication::primaryScreen();
    return screen->grabWindow(0, rect.x(), rect.y(), rect.width(), rect.height());
}
//захват экрана под мышкой
QPixmap CaptureManager::captureAtCursor()
{
    QPoint pos = QCursor::pos();

    int w = 320;
    int h = 90;

    QRect captureRect(
        pos.x() - w / 2,
        pos.y() - h / 2 - 10,
        w,
        h
        );

    QScreen *screen = QGuiApplication::screenAt(pos);
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    if (!screen)
        return QPixmap();

    QRect screenGeometry = screen->geometry();
    QRect fixedRect = captureRect.intersected(screenGeometry);

    return screen->grabWindow(
        0,
        fixedRect.x(),
        fixedRect.y(),
        fixedRect.width(),
        fixedRect.height()
        );
}