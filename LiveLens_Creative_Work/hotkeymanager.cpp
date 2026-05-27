#include "hotkeymanager.h"
#include <QAbstractEventDispatcher>
#include <QDebug>
#include <QApplication>

#if defined(Q_OS_WIN)
#include <Windows.h>
#define HOTKEY_FULLSCREEN 1
#define HOTKEY_REGION     2
#define HOTKEY_POINT      3
#endif

HotkeyManager::HotkeyManager(QObject *parent)
    : QObject(parent)
{
#if defined(Q_OS_WIN)
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
    registerHotkeys();
#endif
}

HotkeyManager::~HotkeyManager()
{
#if defined(Q_OS_WIN)
    QAbstractEventDispatcher::instance()->removeNativeEventFilter(this);
#endif
}

bool HotkeyManager::registerHotkeys()
{
#if defined(Q_OS_WIN)
    HWND hwnd = nullptr; // привязка к потоку
    if (!RegisterHotKey(hwnd, HOTKEY_FULLSCREEN, MOD_CONTROL | MOD_SHIFT, 'F'))
        qWarning() << "Failed to register hotkey Ctrl+Shift+F";
    if (!RegisterHotKey(hwnd, HOTKEY_REGION, MOD_CONTROL | MOD_SHIFT, 'Z'))
        qWarning() << "Failed to register hotkey Ctrl+Shift+Z";
    if (!RegisterHotKey(hwnd, HOTKEY_POINT, MOD_CONTROL | MOD_SHIFT, 'X'))
        qWarning() << "Failed to register hotkey Ctrl+Shift+X";
    return true;
#else
    return false;
#endif
}

#if defined(Q_OS_WIN)
bool HotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_HOTKEY) {
        if (msg->wParam == HOTKEY_FULLSCREEN) {
            emit captureFullScreen();
            return true;
        } else if (msg->wParam == HOTKEY_REGION) {
            emit captureRegion();
            return true;
        } else if (msg->wParam == HOTKEY_POINT) {
            emit capturePoint();
            return true;
        }
    }
    return false;
}
#endif