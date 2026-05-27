#include "popup.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <QTimer>
#include <QMouseEvent>

Popup::Popup(const QString &original, const QString &translated, QWidget *parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TranslucentBackground, false);

    auto *layout = new QVBoxLayout(this);
    auto *origLabel = new QLabel("Оригинал: " + original, this);
    origLabel->setWordWrap(true);
    auto *transLabel = new QLabel("Перевод: " + translated, this);
    transLabel->setWordWrap(true);
    transLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
    layout->addWidget(origLabel);
    layout->addWidget(transLabel);

    setLayout(layout);
    setStyleSheet("background-color: #fef9e7; border: 1px solid #bdc3c7; border-radius: 5px; padding: 8px;");
    adjustSize();

    // Разместить рядом с курсором
    QPoint cursorPos = QCursor::pos();
    QScreen *screen = QGuiApplication::screenAt(cursorPos);
    if (screen) {
        QRect screenRect = screen->geometry();
        int x = cursorPos.x() + 20;
        int y = cursorPos.y() + 20;
        if (x + width() > screenRect.right()) x = cursorPos.x() - width() - 20;
        if (y + height() > screenRect.bottom()) y = cursorPos.y() - height() - 20;
        move(x, y);
    } else {
        move(cursorPos.x() + 20, cursorPos.y() + 20);
    }

    // Автоматически закрыться через 10 секунд или по клику
    QTimer::singleShot(10000, this, &QWidget::close);
    setMouseTracking(true);
    installEventFilter(this);
}

bool Popup::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        close();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}