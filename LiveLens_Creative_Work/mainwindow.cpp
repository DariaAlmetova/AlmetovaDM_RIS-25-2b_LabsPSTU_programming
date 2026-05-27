#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturemanager.h"
#include "ocrengine.h"
#include "hotkeymanager.h"
#include "popup.h"
#include "dictionarydialog.h"

#include <QInputDialog>
#include <QSettings>
#include <QMessageBox>
#include <QTimer>
#include <QScreen>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QGraphicsDropShadowEffect>
#include <QStandardPaths>
#include <QDir>
#include <QRegularExpression>
#include <QCoreApplication>

// ---------------------------------------------------------------------------
// Конструктор
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_hotkeyMgr(new HotkeyManager(this)),
    m_captureMgr(new CaptureManager(this))

{
    ui->setupUi(this);
    loadUserDictionaries();
    setupStyle();   // применяем стиль
    QString tessdataPath = qEnvironmentVariable("TESSDATA_DIR");

    if (tessdataPath.isEmpty()) {
        tessdataPath = QCoreApplication::applicationDirPath() + "/tessdata";
    }

    if (!QDir(tessdataPath).exists()) {
        QMessageBox::critical(
            this,
            "Ошибка Tesseract",
            "Не найдена папка tessdata.\n\n"
            "Положите папку tessdata рядом с exe-файлом или задайте переменную TESSDATA_DIR."
            );
        qApp->quit();
        return;
    }

    OCREngine::initialize(tessdataPath);

    connect(ui->simpleBtn,    &QRadioButton::toggled, this, &MainWindow::onTranslationTypeChanged);
    connect(ui->technicalBtn, &QRadioButton::toggled, this, &MainWindow::onTranslationTypeChanged);
    connect(ui->fullScreenBtn, &QRadioButton::toggled, this, &MainWindow::onCaptureModeChanged);
    connect(ui->regionBtn,     &QRadioButton::toggled, this, &MainWindow::onCaptureModeChanged);
    connect(ui->pointBtn,      &QRadioButton::toggled, this, &MainWindow::onCaptureModeChanged);
    connect(ui->startCaptureBtn, &QPushButton::clicked, this, &MainWindow::on_startCaptureBtn_clicked);

    connect(m_hotkeyMgr, &HotkeyManager::captureFullScreen, this, &MainWindow::captureFullScreen);
    connect(m_hotkeyMgr, &HotkeyManager::captureRegion,     this, &MainWindow::captureRegion);
    connect(m_hotkeyMgr, &HotkeyManager::capturePoint,      this, &MainWindow::capturePoint);

    // setupTrayIcon();   // включите, когда добавите иконку
}

// ---------------------------------------------------------------------------
// Деструктор
// ---------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

// ---------------------------------------------------------------------------
// Стилизация (градиент, крупные шрифты, тени)
// ---------------------------------------------------------------------------
void MainWindow::setupStyle()
{
    setStyleSheet(R"(
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #e8ecf1, stop:1 #cfd8dc);
        }
    )");

    qApp->setStyleSheet(R"(
        * {
            font-size: 15px;
            font-family: "Segoe UI", "Roboto", sans-serif;
        }
        QGroupBox {
            background-color: rgba(255, 255, 255, 0.9);
            border: 1px solid #b0bec5;
            border-radius: 10px;
            margin-top: 18px;
            padding: 14px;
            font-weight: bold;
            font-size: 15px;
            color: #1e2a36;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 8px;
            font-size: 14px;
        }
        QRadioButton {
            spacing: 10px;
            font-size: 15px;
            color: #2c3e50;
            padding: 4px 0;
        }
        QRadioButton::indicator {
            width: 20px;
            height: 20px;
        }
        QRadioButton::indicator:unchecked {
            border: 2px solid #90a4ae;
            border-radius: 10px;
            background-color: #ffffff;
        }
        QRadioButton::indicator:checked {
            border: 2px solid #1976d2;
            border-radius: 10px;
            background-color: #1976d2;
        }
        QPushButton {
            background-color: #1976d2;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #1565c0;
        }
        QPushButton:pressed {
            background-color: #0d47a1;
        }
        QPushButton:disabled {
            background-color: #b0bec5;
            color: #eceff1;
        }
        QLabel#titleLabel {
            font-size: 22px;
            font-weight: bold;
            color: #0d2b45;
            padding: 4px;
            background: transparent;
        }
        QLabel#statusLabel {
            color: #455a64;
            font-style: italic;
            font-size: 14px;
            padding: 6px;
            background: transparent;
        }
        Line {
            color: #cfd8dc;
        }
    )");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 40));
    ui->centralwidget->setGraphicsEffect(shadow);
}

// ---------------------------------------------------------------------------
// Иконка в трее (пока не активна)
// ---------------------------------------------------------------------------
void MainWindow::setupTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(QIcon(":/icon.png"), this);
    m_trayIcon->setToolTip("Screen Translator");
    auto *menu = new QMenu(this);
    QAction *showAction = menu->addAction("Показать");
    connect(showAction, &QAction::triggered, this, &MainWindow::show);
    QAction *exitAction = menu->addAction("Выход");
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
    m_trayIcon->setContextMenu(menu);
    m_trayIcon->show();
}

// ---------------------------------------------------------------------------
// Слоты
// ---------------------------------------------------------------------------
void MainWindow::onTranslationTypeChanged()
{
    if (ui->simpleBtn->isChecked()) {
        m_translationType = 0;
        m_currentTranslator = &m_onlineTranslator;
        ui->statusLabel->setText("Обычный онлайн-перевод выбран.");
    } else {
        m_translationType = 1;
        m_currentTranslator = &m_technicalTranslator;
        ui->statusLabel->setText("Технический перевод выбран.");
    }
}

void MainWindow::onCaptureModeChanged()
{
    if (ui->fullScreenBtn->isChecked()) m_captureMode = 0;
    else if (ui->regionBtn->isChecked()) m_captureMode = 1;
    else if (ui->pointBtn->isChecked()) m_captureMode = 2;
    else m_captureMode = -1;
    ui->startCaptureBtn->setEnabled(m_captureMode >= 0);
    ui->statusLabel->setText("Режим захвата выбран. Нажмите кнопку или используйте горячую клавишу.");
}

void MainWindow::on_startCaptureBtn_clicked()
{
    if (m_isCapturing) return;
    if (m_captureMode == 0) captureFullScreen();
    else if (m_captureMode == 1) captureRegion();
    else if (m_captureMode == 2) capturePoint();
}

void MainWindow::captureFullScreen()
{
    if (m_isCapturing) return;

    m_isCapturing = true;
    hide();

    QTimer::singleShot(500, this, [this]() {
        QScreen *screen = QGuiApplication::primaryScreen();

        if (!screen) {
            show();
            ui->statusLabel->setText("Экран не найден.");
            m_isCapturing = false;
            return;
        }

        QPixmap pix = screen->grabWindow(0);
        show();

        if (!pix.isNull()) {
            translateAndShow(pix.toImage(), false);
        } else {
            ui->statusLabel->setText("Не удалось сделать скриншот.");
            m_isCapturing = false;
        }
    });
}

void MainWindow::captureRegion()
{
    if (m_isCapturing) return;
    m_isCapturing = true;

    hide();
    QTimer::singleShot(500, this, [this]() {
        QPixmap region = m_captureMgr->captureRegion();
        show();

        if (!region.isNull()) {
            translateAndShow(region.toImage(), false);
        } else {
            ui->statusLabel->setText("Захват области отменён.");
        }

        m_isCapturing = false;
    });
}

void MainWindow::capturePoint()
{
    captureRegion();
}

void MainWindow::translateAndShow(const QImage &image, bool singleWord)
{
    qDebug() << "translateAndShow called, image size:" << image.size();

    if (image.isNull()) {
        ui->statusLabel->setText("Изображение пустое");
        m_isCapturing = false;
        return;
    }

    QString text;

    if (singleWord)
        text = OCREngine::instance()->recognizeWord(image);
    else
        text = OCREngine::instance()->recognize(image);

    qDebug() << "Recognized text: [" << text << "]";

    if (!singleWord) {
        text = filterTechnicalTextOnly(text);
        qDebug() << "Filtered technical text: [" << text << "]";
    }

    if (text.trimmed().isEmpty()) {
        ui->statusLabel->setText("Текст не распознан!");
        m_isCapturing = false;
        return;
    }

    QString translated;

    QString normalizedText = text.trimmed();
    QStringList words = normalizedText.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    // Если выделено одно слово — сразу берём из пользовательского словаря
    if (words.size() == 1) {
        QString dictionaryTranslation = findInUserDictionary(text);

        if (!dictionaryTranslation.isEmpty()) {
            translated = dictionaryTranslation;
        } else {
            translated = m_currentTranslator->translate(text);
        }
    } else {
        QMap<QString, QString> markers;
        QString preparedText = prepareTextWithUserDictionaryMarkers(text, markers);

        translated = m_currentTranslator->translate(preparedText);
        translated = restoreUserDictionaryMarkers(translated, markers);
    }

    showPopup(text, translated);

    ui->statusLabel->setText("Готово");
    m_isCapturing = false;
}

void MainWindow::showPopup(const QString &original, const QString &translated)
{
    auto *popup = new Popup(original, translated);
    popup->show();
}

QString MainWindow::filterTechnicalTextOnly(const QString &text) const
{
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    QStringList result;

    QSet<QString> techWords = {
        // Базовые слова из твоего проекта
        "driver", "drivers",
        "mouse",
        "screen", "display", "monitor",
        "keyboard", "keyboards",
        "computer",
        "table", "tables",
        "window", "windows",
        "button", "buttons",
        "exit",
        "application", "applications",
        "error", "errors",
        "file", "files",
        "settings",

        // Интерфейс
        "menu", "toolbar", "panel", "tab", "tabs",
        "dialog", "checkbox", "textbox", "field", "form",
        "label", "popup", "tooltip", "icon", "cursor",

        // Система
        "process", "processes",
        "thread", "threads",
        "service", "services",
        "task", "tasks",
        "memory", "cache", "buffer",
        "clipboard", "registry", "kernel",

        // Файлы
        "folder", "folders",
        "directory", "directories",
        "path", "paths",
        "extension", "archive", "backup",

        // Сеть
        "server", "servers",
        "client", "clients",
        "network", "networks",
        "request", "requests",
        "response", "responses",
        "connection", "connections",
        "socket", "port", "ports",
        "host", "protocol", "url", "ip", "address",

        // Программирование
        "program", "programs",
        "software",
        "module", "modules",
        "component", "components",
        "library", "libraries",
        "framework",
        "function", "functions",
        "method", "methods",
        "class", "classes",
        "object", "objects",
        "variable", "variables",
        "parameter", "parameters",
        "argument", "arguments",
        "event", "events",
        "handler", "callback",

        // Ошибки и отладка
        "warning", "warnings",
        "exception", "exceptions",
        "debug", "debugging",
        "crash", "log", "logs", "trace", "assertion",

        // Действия
        "install", "installed",
        "update", "updated",
        "download", "upload",
        "save", "load",
        "open", "close",
        "run", "start", "stop", "restart",
        "execute", "select", "click", "press",
        "capture", "recognize", "translate"
    };

    for (QString line : lines) {
        line = line.trimmed();

        if (line.length() < 4)
            continue;

        // Убираем строки с кириллицей, потому что переводим английский -> русский
        if (line.contains(QRegularExpression("[А-Яа-яЁё]")))
            continue;

        // Разбиваем строку на английские слова
        QString normalized = line.toLower();
        normalized.replace(QRegularExpression("[^a-zA-Z0-9 ]"), " ");

        QStringList words = normalized.split(' ', Qt::SkipEmptyParts);

        if (words.isEmpty())
            continue;

        int techMatches = 0;
        int latinWords = 0;
        int weirdLongWords = 0;
        int oneLetterWords = 0;

        for (const QString &word : words) {
            if (word.length() == 1)
                oneLetterWords++;

            if (word.length() > 16)
                weirdLongWords++;

            if (word.contains(QRegularExpression("^[a-zA-Z]+$")))
                latinWords++;

            if (techWords.contains(word))
                techMatches++;
        }

        // Главное условие: в строке должно быть хотя бы одно техническое слово
        if (techMatches == 0)
            continue;

        // Отсекаем строки, где много одиночных букв, типа "T R R S S R"
        if (oneLetterWords > words.size() / 2)
            continue;

        // Отсекаем строки с длинными OCR-мутантами
        if (weirdLongWords > 1)
            continue;

        // Если строка длинная, но техническое слово всего одно, это может быть мусор
        if (line.length() > 120 && techMatches < 2)
            continue;

        result << line;
    }

    QString filtered = result.join("\n").trimmed();

    if (filtered.length() > 1000)
        filtered = filtered.left(1000);

    return filtered;
}

void MainWindow::addWordToDictionary()
{
    if (m_isCapturing) return;

    m_isCapturing = true;
    ui->statusLabel->setText("Выделите слово, которое нужно добавить в словарь...");

    hide();

    QTimer::singleShot(500, this, [this]() {
        QPixmap region = m_captureMgr->captureRegion();
        show();

        if (region.isNull()) {
            ui->statusLabel->setText("Добавление слова отменено.");
            m_isCapturing = false;
            return;
        }

        QString recognized = OCREngine::instance()->recognizeWord(region.toImage());
        QString word = normalizeDictionaryWord(recognized);

        if (word.isEmpty()) {
            QMessageBox::warning(this, "Словарь", "Не удалось распознать слово.");
            ui->statusLabel->setText("Слово не распознано.");
            m_isCapturing = false;
            return;
        }

        bool ok = false;

        QString translation = QInputDialog::getText(
            this,
            "Добавить в словарь",
            QString("Распознано слово: %1\nВведите перевод:").arg(word),
            QLineEdit::Normal,
            "",
            &ok
            );

        if (!ok || translation.trimmed().isEmpty()) {
            ui->statusLabel->setText("Добавление слова отменено.");
            m_isCapturing = false;
            return;
        }

        word = word.toLower().trimmed();
        translation = translation.trimmed();

        if (ui->simpleBtn->isChecked()) {
            m_simpleUserDict[word] = translation;
            ui->statusLabel->setText("Слово добавлено в обычный словарь.");
        } else {
            m_technicalUserDict[word] = translation;
            ui->statusLabel->setText("Слово добавлено в технический словарь.");
        }

        saveUserDictionaries();

        QMessageBox::information(
            this,
            "Словарь",
            QString("Добавлено:\n%1 → %2").arg(word, translation)
            );

        m_isCapturing = false;
    });
}

QString MainWindow::normalizeDictionaryWord(const QString &text) const
{
    QString result = text.trimmed();

    result.replace(QRegularExpression("[^A-Za-zА-Яа-яЁё0-9_-]"), " ");
    QStringList words = result.split(' ', Qt::SkipEmptyParts);

    if (words.isEmpty())
        return "";

    return words.first().trimmed();
}

QString MainWindow::findInUserDictionary(const QString &text) const
{
    QString word = normalizeDictionaryWord(text).toLower();

    if (word.isEmpty())
        return "";

    if (ui->simpleBtn->isChecked()) {
        if (m_simpleUserDict.contains(word))
            return m_simpleUserDict.value(word);
    } else {
        if (m_technicalUserDict.contains(word))
            return m_technicalUserDict.value(word);
    }

    return "";
}

void MainWindow::saveUserDictionaries()
{
    QSettings settings("ScreenTranslator", "ScreenTranslator");

    settings.beginGroup("SimpleUserDictionary");
    settings.remove("");

    for (auto it = m_simpleUserDict.begin(); it != m_simpleUserDict.end(); ++it) {
        settings.setValue(it.key(), it.value());
    }

    settings.endGroup();

    settings.beginGroup("TechnicalUserDictionary");
    settings.remove("");

    for (auto it = m_technicalUserDict.begin(); it != m_technicalUserDict.end(); ++it) {
        settings.setValue(it.key(), it.value());
    }

    settings.endGroup();
}

void MainWindow::loadUserDictionaries()
{
    QSettings settings("ScreenTranslator", "ScreenTranslator");

    settings.beginGroup("SimpleUserDictionary");

    for (const QString &key : settings.childKeys()) {
        m_simpleUserDict[key] = settings.value(key).toString();
    }

    settings.endGroup();

    settings.beginGroup("TechnicalUserDictionary");

    for (const QString &key : settings.childKeys()) {
        m_technicalUserDict[key] = settings.value(key).toString();
    }

    settings.endGroup();
}

void MainWindow::on_changeWord_btn_clicked()
{
    addWordToDictionary();
}

QString MainWindow::translateWordByUserDictionary(const QString &word) const
{
    QString cleanWord = normalizeDictionaryWord(word).toLower();

    if (cleanWord.isEmpty())
        return "";

    if (ui->simpleBtn->isChecked()) {
        return m_simpleUserDict.value(cleanWord, "");
    }

    return m_technicalUserDict.value(cleanWord, "");
}

QString MainWindow::applyUserDictionaryToTranslation(const QString &originalText,
                                                     const QString &translatedText) const
{
    QString result = translatedText;

    QString normalizedOriginal = originalText;
    normalizedOriginal.replace(QRegularExpression("[^A-Za-zА-Яа-яЁё0-9_-]"), " ");

    QStringList originalWords = normalizedOriginal.split(' ', Qt::SkipEmptyParts);

    for (const QString &originalWord : originalWords) {
        QString userTranslation = translateWordByUserDictionary(originalWord);

        if (userTranslation.isEmpty())
            continue;

        QString cleanOriginal = normalizeDictionaryWord(originalWord);

        if (cleanOriginal.isEmpty())
            continue;

        QRegularExpression originalRegex(
            "(^|[^\\p{L}\\p{N}_])" +
                QRegularExpression::escape(cleanOriginal) +
                "($|[^\\p{L}\\p{N}_])",
            QRegularExpression::CaseInsensitiveOption
            );

        result.replace(originalRegex, "\\1" + userTranslation + "\\2");
    }

    return result;
}

QString MainWindow::applyUserDictionaryToOriginalText(const QString &originalText) const
{
    QString result = originalText;

    const QMap<QString, QString> &dict =
        ui->simpleBtn->isChecked() ? m_simpleUserDict : m_technicalUserDict;

    for (auto it = dict.begin(); it != dict.end(); ++it) {
        QRegularExpression regex(
            "(^|[^A-Za-zА-Яа-яЁё0-9_])" +
                QRegularExpression::escape(it.key()) +
                "($|[^A-Za-zА-Яа-яЁё0-9_])",
            QRegularExpression::CaseInsensitiveOption
            );

        result.replace(regex, "\\1" + it.value() + "\\2");
    }

    return result;
}

QString MainWindow::prepareTextWithUserDictionaryMarkers(const QString &originalText,
                                                         QMap<QString, QString> &markers) const
{
    QString result = originalText;
    markers.clear();

    const QMap<QString, QString> &dict =
        ui->simpleBtn->isChecked() ? m_simpleUserDict : m_technicalUserDict;

    int index = 0;

    for (auto it = dict.begin(); it != dict.end(); ++it) {
        QString word = it.key().trimmed().toLower();
        QString userTranslation = it.value().trimmed();

        if (word.isEmpty() || userTranslation.isEmpty())
            continue;

        QString marker = QString(" DICTMARKER%1 ").arg(index++);

        QRegularExpression regex(
            "(^|[^A-Za-zА-Яа-яЁё0-9_])" +
                QRegularExpression::escape(word) +
                "($|[^A-Za-zА-Яа-яЁё0-9_])",
            QRegularExpression::CaseInsensitiveOption
            );

        if (result.contains(regex)) {
            result.replace(regex, "\\1" + marker.trimmed() + "\\2");
            markers[marker.trimmed().toLower()] = userTranslation;
        }
    }

    return result;
}

QString MainWindow::restoreUserDictionaryMarkers(const QString &translatedText,
                                                 const QMap<QString, QString> &markers) const
{
    QString result = translatedText;

    for (auto it = markers.begin(); it != markers.end(); ++it) {
        QRegularExpression regex(
            QRegularExpression::escape(it.key()),
            QRegularExpression::CaseInsensitiveOption
            );

        result.replace(regex, it.value());
    }

    return result;
}
void MainWindow::on_openDictBtn_clicked()
{
    if (ui->simpleBtn->isChecked()) {
        DictionaryDialog dialog(&m_simpleUserDict, "Обычный пользовательский словарь", this);
        dialog.exec();
    } else {
        DictionaryDialog dialog(&m_technicalUserDict, "Технический пользовательский словарь", this);
        dialog.exec();
    }

    saveUserDictionaries();

    ui->statusLabel->setText("Словарь обновлён.");
}

