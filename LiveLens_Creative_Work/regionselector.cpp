#include "regionselector.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>
#include <QEventLoop>
#include <QThread>


RegionSelector::RegionSelector(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setWindowOpacity(0.3);
    setStyleSheet("background-color: black;");
    QScreen *screen = QGuiApplication::primaryScreen();
    setGeometry(screen->geometry());
    setCursor(Qt::CrossCursor);
    m_selectionDone = false;
}

QRect RegionSelector::getSelection()
{
    m_selection = QRect();
    m_selectionDone = false;
    show();
    raise();
    activateWindow();

    while (!m_selectionDone) {
        QApplication::processEvents();
        QThread::msleep(10);
    }

    hide();
    return m_selection;
}

void RegionSelector::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(QPen(Qt::red, 2, Qt::DashLine));
    p.setBrush(Qt::NoBrush);
    if (!m_selection.isNull() && m_selection.width() > 0 && m_selection.height() > 0) {
        p.drawRect(m_selection);
    }
}

void RegionSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = event->pos();
        m_selecting = true;
        m_selection = QRect();
        update();
    }
}

void RegionSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (m_selecting) {
        m_selection = QRect(m_startPoint, event->pos()).normalized();
        update();
    }
}

void RegionSelector::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_selecting) {
        m_selecting = false;
        if (m_selection.width() >= 10 && m_selection.height() >= 10) {
            m_selectionDone = true;
        }else {
            m_selection = QRect();
            m_selectionDone = true;
        }
        update();
    }
}

void RegionSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        m_selection = QRect();
        m_selectionDone = true;
        close();
    }
}