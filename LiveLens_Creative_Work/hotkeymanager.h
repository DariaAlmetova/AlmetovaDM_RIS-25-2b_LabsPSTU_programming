#pragma once
#include <QObject>
#include <QAbstractNativeEventFilter>

class HotkeyManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();
    bool registerHotkeys();

signals:
    void captureFullScreen();
    void captureRegion();
    void capturePoint();

protected:
#if defined(Q_OS_WIN)
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
#endif
};