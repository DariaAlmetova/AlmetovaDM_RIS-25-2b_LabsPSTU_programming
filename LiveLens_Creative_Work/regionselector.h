#pragma once
#include <QWidget>
#include <QRect>
#include <QThread>

class RegionSelector : public QWidget
{
    Q_OBJECT
public:
    explicit RegionSelector(QWidget *parent = nullptr);
    QRect getSelection();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void selectionDone();
private:
    QRect m_selection;
    bool m_selecting = false;
    bool m_selectionDone;
    QPoint m_startPoint;
};