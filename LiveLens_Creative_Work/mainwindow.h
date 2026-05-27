#pragma once
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMap>
#include "translator.h"
#include "ocrengine.h"

namespace Ui { class MainWindow; }
class HotkeyManager;
class CaptureManager;
class OCREngine;
class Popup;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTranslationTypeChanged();
    void onCaptureModeChanged();
    void captureFullScreen();
    void captureRegion();
    void capturePoint();    
    void on_startCaptureBtn_clicked();
    void on_changeWord_btn_clicked();
    void on_openDictBtn_clicked();

private:
    void setupStyle();   // стилизация интерфейса
    void setupTrayIcon(); // иконка в трее (пока не используется)

    Ui::MainWindow *ui;

    int m_translationType = 0;   // 0 – простой, 1 – технический
    int m_captureMode = -1;      // 0 – экран, 1 – область, 2 – точечный

    QSystemTrayIcon *m_trayIcon = nullptr;
    HotkeyManager    *m_hotkeyMgr  = nullptr;
    CaptureManager   *m_captureMgr = nullptr;

    SimpleTranslator     m_simpleTranslator;
    TechnicalTranslator  m_technicalTranslator;
    OnlineTranslator     m_onlineTranslator;

    Translator          *m_currentTranslator = &m_onlineTranslator;
    bool m_isCapturing = false;
    QString filterTechnicalTextOnly(const QString &text) const;
    void translateAndShow(const QImage &image, bool singleWord = false);
    void showPopup(const QString &original, const QString &translated);

    void addWordToDictionary();
    void loadUserDictionaries();
    void saveUserDictionaries();

    QString normalizeDictionaryWord(const QString &text) const;
    QString findInUserDictionary(const QString &text) const;

    QMap<QString, QString> m_simpleUserDict;
    QMap<QString, QString> m_technicalUserDict;

    QString applyUserDictionaryToTranslation(const QString &originalText,
                                             const QString &translatedText) const;

    QString translateWordByUserDictionary(const QString &word) const;
    QString applyUserDictionaryToOriginalText(const QString &originalText) const;

    QString prepareTextWithUserDictionaryMarkers(const QString &originalText,
                                                 QMap<QString, QString> &markers) const;

    QString restoreUserDictionaryMarkers(const QString &translatedText,
                                         const QMap<QString, QString> &markers) const;
};