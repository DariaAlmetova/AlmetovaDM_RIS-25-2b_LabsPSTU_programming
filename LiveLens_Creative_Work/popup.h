#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>

class Popup : public QWidget
{
    Q_OBJECT
public:
    explicit Popup(const QString &original, const QString &translated, QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};