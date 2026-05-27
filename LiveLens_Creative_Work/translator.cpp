#include "translator.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>


SimpleTranslator::SimpleTranslator()
{
    m_dict = {
        {"driver", "водитель"},
        {"mouse", "мышь"},
        {"screen", "экран"},
        {"keyboard", "клавиатура"},
        {"computer", "компьютер"},
        {"table", "таблица"},
        {"window", "окно"},
        {"button", "кнопка"},
        {"exit", "выход"},
        {"application", "приложение"},
        {"error", "ошибка"},
        {"file", "файл"},
        {"settings", "настройки"}
    };
}

QString SimpleTranslator::translate(const QString &text)
{
    QStringList words = text.split(' ', Qt::SkipEmptyParts);
    QStringList result;
    for (const QString &word : words) {
        QString cleanWord = word.toLower().remove(QRegularExpression("[^a-zA-Z]"));
        if (m_dict.contains(cleanWord)) {
            result << m_dict[cleanWord];
        } else {
            result << word;
        }
    }
    return result.join(' ');
}

TechnicalTranslator::TechnicalTranslator()
{
    // Интерфейс
    m_russianTechnicalDict["окно"] = "окно интерфейса";
    m_russianTechnicalDict["окна"] = "окна интерфейса";
    m_russianTechnicalDict["окне"] = "окне интерфейса";
    m_russianTechnicalDict["окном"] = "окном интерфейса";

    m_russianTechnicalDict["кнопка"] = "кнопка интерфейса";
    m_russianTechnicalDict["кнопки"] = "кнопки интерфейса";
    m_russianTechnicalDict["кнопку"] = "кнопку интерфейса";
    m_russianTechnicalDict["кнопками"] = "кнопками интерфейса";

    m_russianTechnicalDict["меню"] = "меню приложения";
    m_russianTechnicalDict["панель"] = "панель интерфейса";
    m_russianTechnicalDict["вкладка"] = "вкладка интерфейса";
    m_russianTechnicalDict["вкладки"] = "вкладки интерфейса";
    m_russianTechnicalDict["диалог"] = "диалоговое окно";
    m_russianTechnicalDict["форма"] = "форма ввода";
    m_russianTechnicalDict["поле"] = "поле ввода";
    m_russianTechnicalDict["поля"] = "поля ввода";
    m_russianTechnicalDict["значок"] = "иконка интерфейса";
    m_russianTechnicalDict["курсор"] = "курсор мыши";
    m_russianTechnicalDict["ярлык"] = "сочетание клавиш";

    // Устройства
    m_russianTechnicalDict["водитель"] = "драйвер";
    m_russianTechnicalDict["водителя"] = "драйвера";
    m_russianTechnicalDict["водителем"] = "драйвером";
    m_russianTechnicalDict["водители"] = "драйверы";
    m_russianTechnicalDict["водителями"] = "драйверами";

    m_russianTechnicalDict["мышь"] = "манипулятор мышь";
    m_russianTechnicalDict["мыши"] = "манипулятора мышь";
    m_russianTechnicalDict["мышью"] = "манипулятором мышь";

    m_russianTechnicalDict["экран"] = "дисплей";
    m_russianTechnicalDict["экрана"] = "дисплея";
    m_russianTechnicalDict["экране"] = "дисплее";

    m_russianTechnicalDict["клавиатура"] = "клавиатура (устройство ввода)";
    m_russianTechnicalDict["клавиатуру"] = "клавиатуру (устройство ввода)";
    m_russianTechnicalDict["клавиатуры"] = "клавиатуры (устройство ввода)";
    m_russianTechnicalDict["клавиатурой"] = "клавиатурой (устройство ввода)";

    m_russianTechnicalDict["принтер"] = "печатающее устройство";
    m_russianTechnicalDict["сканер"] = "сканирующее устройство";
    m_russianTechnicalDict["устройство"] = "компьютерное устройство";
    m_russianTechnicalDict["устройства"] = "компьютерные устройства";

    // Система
    m_russianTechnicalDict["процесс"] = "процесс выполнения";
    m_russianTechnicalDict["процессы"] = "процессы выполнения";
    m_russianTechnicalDict["поток"] = "поток выполнения";
    m_russianTechnicalDict["потоки"] = "потоки выполнения";
    m_russianTechnicalDict["служба"] = "системная служба";
    m_russianTechnicalDict["память"] = "оперативная память";
    m_russianTechnicalDict["кэш"] = "кэш-память";
    m_russianTechnicalDict["буфер"] = "буфер данных";
    m_russianTechnicalDict["буфер обмена"] = "системный буфер обмена";
    m_russianTechnicalDict["реестр"] = "системный реестр";
    m_russianTechnicalDict["ядро"] = "ядро операционной системы";

    // Файлы и данные
    //m_russianTechnicalDict["файл"] = "файл";
    //m_russianTechnicalDict["файлы"] = "файлы";
    m_russianTechnicalDict["папка"] = "каталог";
    m_russianTechnicalDict["папку"] = "каталог";
    m_russianTechnicalDict["папки"] = "каталоги";
    m_russianTechnicalDict["каталог"] = "каталог файловой системы";
    m_russianTechnicalDict["путь"] = "путь к файлу";
    m_russianTechnicalDict["архив"] = "файловый архив";
    m_russianTechnicalDict["резервная копия"] = "резервная копия данных";

    m_russianTechnicalDict["база данных"] = "база данных";
    m_russianTechnicalDict["таблица"] = "таблица базы данных";
    m_russianTechnicalDict["таблицу"] = "таблицу базы данных";
    m_russianTechnicalDict["таблицы"] = "таблицы базы данных";
    m_russianTechnicalDict["таблице"] = "таблице базы данных";
    m_russianTechnicalDict["запись"] = "запись базы данных";
    //m_russianTechnicalDict["запрос"] = "запрос к базе данных";
    //m_russianTechnicalDict["запросы"] = "запросы к базе данных";

    // Сеть
    //m_russianTechnicalDict["сервер"] = "сервер";
    m_russianTechnicalDict["клиент"] = "клиентское приложение";
    m_russianTechnicalDict["сеть"] = "компьютерная сеть";
    m_russianTechnicalDict["соединение"] = "сетевое соединение";
    m_russianTechnicalDict["разъем"] = "сетевой сокет";
    m_russianTechnicalDict["порт"] = "сетевой порт";
    m_russianTechnicalDict["хост"] = "сетевой узел";
    m_russianTechnicalDict["протокол"] = "сетевой протокол";
    m_russianTechnicalDict["адрес"] = "сетевой адрес";
    m_russianTechnicalDict["ответ"] = "ответ сервера";

    // Программирование
    //m_russianTechnicalDict["приложение"] = "программное приложение";
    //m_russianTechnicalDict["приложения"] = "программные приложения";
    //m_russianTechnicalDict["программа"] = "программное приложение";
    m_russianTechnicalDict["модуль"] = "программный модуль";
    m_russianTechnicalDict["компонент"] = "программный компонент";
    m_russianTechnicalDict["библиотека"] = "программная библиотека";
    m_russianTechnicalDict["фреймворк"] = "программный фреймворк";

    m_russianTechnicalDict["функция"] = "программная функция";
    m_russianTechnicalDict["метод"] = "метод класса";
    m_russianTechnicalDict["класс"] = "класс программы";
    m_russianTechnicalDict["объект"] = "объект программы";
    m_russianTechnicalDict["переменная"] = "программная переменная";
    m_russianTechnicalDict["параметр"] = "параметр функции";
    m_russianTechnicalDict["аргумент"] = "аргумент функции";
    m_russianTechnicalDict["событие"] = "событие программы";
    m_russianTechnicalDict["обработчик"] = "обработчик события";

    // Ошибки и отладка
    m_russianTechnicalDict["ошибка"] = "системная ошибка";
    m_russianTechnicalDict["ошибку"] = "системную ошибку";
    m_russianTechnicalDict["ошибки"] = "системные ошибки";

    m_russianTechnicalDict["предупреждение"] = "системное предупреждение";
    m_russianTechnicalDict["исключение"] = "программное исключение";
    m_russianTechnicalDict["сбой"] = "сбой программы";
    m_russianTechnicalDict["журнал"] = "системный журнал";
    m_russianTechnicalDict["отладка"] = "отладка программы";
    m_russianTechnicalDict["трассировка"] = "трассировка выполнения";
}

QString TechnicalTranslator::translate(const QString &text)
{
    qDebug() << "TECHNICAL TRANSLATOR USED";

    QString preparedText = prepareEnglishText(text);

    qDebug() << "Prepared English text:" << preparedText;

    QString translated = m_onlineTranslator.translate(preparedText);

    if (translated.startsWith("Ошибка"))
        return translated;

    return applyRussianTechnicalTerms(translated);
}
QString TechnicalTranslator::prepareEnglishText(const QString &text) const
{
    QString result = text;

    QMap<QString, QString> hints;
    // Интерфейс
    hints["window"] = "interface window";
    hints["windows"] = "interface windows";
    hints["button"] = "interface button";
    hints["buttons"] = "interface buttons";
    hints["menu"] = "application menu";
    hints["toolbar"] = "application toolbar";
    hints["panel"] = "interface panel";
    hints["tab"] = "interface tab";
    hints["tabs"] = "interface tabs";
    hints["dialog"] = "dialog window";
    hints["checkbox"] = "checkbox element";
    hints["radiobutton"] = "radio button element";
    hints["textbox"] = "text input field";
    hints["field"] = "input field";
    hints["fields"] = "input fields";
    hints["form"] = "input form";
    hints["label"] = "text label";
    hints["popup"] = "popup window";
    hints["tooltip"] = "tooltip hint";
    hints["icon"] = "interface icon";
    hints["cursor"] = "mouse cursor";
    hints["shortcut"] = "keyboard shortcut";

    // Устройства ввода/вывода
    hints["keyboard"] = "input keyboard";
    hints["keyboards"] = "input keyboards";
    hints["mouse"] = "computer mouse";
    hints["screen"] = "display screen";
    hints["display"] = "computer display";
    hints["monitor"] = "computer monitor";
    hints["printer"] = "printing device";
    hints["scanner"] = "scanning device";
    hints["device"] = "computer device";
    hints["devices"] = "computer devices";

    // Система и ОС
    hints["driver"] = "software driver";
    hints["drivers"] = "software drivers";
    hints["process"] = "operating system process";
    hints["processes"] = "operating system processes";
    hints["thread"] = "execution thread";
    hints["threads"] = "execution threads";
    hints["service"] = "system service";
    hints["services"] = "system services";
    hints["task"] = "system task";
    hints["tasks"] = "system tasks";
    hints["memory"] = "computer memory";
    hints["cache"] = "cache memory";
    hints["buffer"] = "data buffer";
    hints["clipboard"] = "system clipboard";
    hints["registry"] = "system registry";
    hints["filesystem"] = "file system";
    hints["kernel"] = "operating system kernel";

    // Файлы и данные
    hints["file"] = "computer file";
    hints["files"] = "computer files";
    hints["folder"] = "file directory";
    hints["folders"] = "file directories";
    hints["directory"] = "file directory";
    hints["directories"] = "file directories";
    hints["path"] = "file path";
    hints["paths"] = "file paths";
    hints["extension"] = "file extension";
    hints["archive"] = "file archive";
    hints["backup"] = "backup copy";
    hints["database"] = "database";
    hints["table"] = "database table";
    hints["tables"] = "database tables";
    hints["record"] = "database record";
    hints["records"] = "database records";
    hints["query"] = "database query";
    hints["queries"] = "database queries";

    // Сеть
    hints["server"] = "network server";
    hints["servers"] = "network servers";
    hints["client"] = "client application";
    hints["clients"] = "client applications";
    hints["network"] = "computer network";
    hints["networks"] = "computer networks";
    hints["request"] = "network request";
    hints["requests"] = "network requests";
    hints["response"] = "server response";
    hints["responses"] = "server responses";
    hints["connection"] = "network connection";
    hints["connections"] = "network connections";
    hints["socket"] = "network socket";
    hints["port"] = "network port";
    hints["ports"] = "network ports";
    hints["host"] = "network host";
    hints["protocol"] = "network protocol";
    hints["url"] = "URL address";
    hints["ip"] = "IP address";
    hints["address"] = "network address";

    // Программирование
    hints["application"] = "software application";
    hints["applications"] = "software applications";
    hints["software"] = "software";
    hints["module"] = "software module";
    hints["modules"] = "software modules";
    hints["component"] = "software component";
    hints["components"] = "software components";
    hints["library"] = "software library";
    hints["libraries"] = "software libraries";
    hints["framework"] = "software framework";
    hints["function"] = "program function";
    hints["functions"] = "program functions";
    hints["method"] = "class method";
    hints["methods"] = "class methods";
    hints["class"] = "program class";
    hints["classes"] = "program classes";
    hints["object"] = "program object";
    hints["objects"] = "program objects";
    hints["variable"] = "program variable";
    hints["variables"] = "program variables";
    hints["parameter"] = "function parameter";
    hints["parameters"] = "function parameters";
    hints["argument"] = "function argument";
    hints["arguments"] = "function arguments";
    hints["event"] = "program event";
    hints["events"] = "program events";
    hints["handler"] = "event handler";
    hints["callback"] = "callback function";

    // Ошибки и отладка
    hints["error"] = "system error";
    hints["errors"] = "system errors";
    hints["warning"] = "software warning";
    hints["warnings"] = "software warnings";
    hints["exception"] = "program exception";
    hints["exceptions"] = "program exceptions";
    hints["debug"] = "software debugging";
    hints["debugging"] = "software debugging";
    hints["crash"] = "program crash";
    hints["log"] = "system log";
    hints["logs"] = "system logs";
    hints["trace"] = "debug trace";
    hints["assertion"] = "debug assertion";

    // Действия
    hints["install"] = "install software";
    hints["installed"] = "installed software";
    hints["update"] = "software update";
    hints["updated"] = "updated software";
    hints["download"] = "download data";
    hints["upload"] = "upload data";
    hints["save"] = "save data";
    hints["load"] = "load data";
    hints["open"] = "open file";
    hints["close"] = "close window";
    hints["run"] = "run program";
    hints["start"] = "start program";
    hints["stop"] = "stop program";
    hints["restart"] = "restart program";
    hints["execute"] = "execute command";
    hints["select"] = "select item";
    hints["click"] = "click button";
    hints["press"] = "press key";
    hints["capture"] = "capture screen area";
    hints["recognize"] = "recognize text";
    hints["translate"] = "translate text";

    for (auto it = hints.begin(); it != hints.end(); ++it) {
        QString pattern =
            "(^|[^A-Za-z0-9_])" +
            QRegularExpression::escape(it.key()) +
            "($|[^A-Za-z0-9_])";

        QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);

        result.replace(regex, "\\1" + it.value() + "\\2");
    }

    return result;
}

QString TechnicalTranslator::applyRussianTechnicalTerms(const QString &text) const
{
    QString result = text;

    for (auto it = m_russianTechnicalDict.begin(); it != m_russianTechnicalDict.end(); ++it) {
        QString pattern =
            "(^|[^\\p{L}\\p{N}_])" +
            QRegularExpression::escape(it.key()) +
            "($|[^\\p{L}\\p{N}_])";

        QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);

        result.replace(regex, "\\1" + it.value() + "\\2");
    }

    return result;
}

OnlineTranslator::OnlineTranslator()
{
}

void OnlineTranslator::setServerUrl(const QString &url)
{
    m_serverUrl = url;
}

void OnlineTranslator::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void OnlineTranslator::setSourceLanguage(const QString &source)
{
    m_sourceLanguage = source;
}

void OnlineTranslator::setTargetLanguage(const QString &target)
{
    m_targetLanguage = target;
}

QString OnlineTranslator::translate(const QString &text)
{
    if (text.trimmed().isEmpty())
        return "";

    QNetworkAccessManager manager;

    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["q"] = text;
    body["source"] = m_sourceLanguage;
    body["target"] = m_targetLanguage;
    body["format"] = "text";

    if (!m_apiKey.trimmed().isEmpty())
        body["api_key"] = m_apiKey;

    QJsonDocument doc(body);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    QNetworkReply *reply = manager.post(request, data);

    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    timer.start(15000);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    loop.exec();

    if (timer.isActive()) {
        timer.stop();
    } else {
        reply->abort();
        reply->deleteLater();
        return "Ошибка: превышено время ожидания перевода";
    }

    if (reply->error() != QNetworkReply::NoError) {
        QString error = reply->errorString();
        reply->deleteLater();
        return "Ошибка перевода: " + error;
    }

    QByteArray response = reply->readAll();
    reply->deleteLater();

    QJsonParseError parseError;
    QJsonDocument responseDoc = QJsonDocument::fromJson(response, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        qDebug() << "Response:" << response;
        return "Ошибка: неверный ответ сервера";
    }

    QJsonObject obj = responseDoc.object();

    if (obj.contains("translatedText"))
        return obj["translatedText"].toString();

    if (obj.contains("error"))
        return "Ошибка API: " + obj["error"].toString();

    qDebug() << "Unexpected response:" << response;
    return "Ошибка: перевод не найден в ответе сервера";
}