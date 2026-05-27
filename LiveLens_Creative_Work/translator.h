#pragma once

#include <QString>
#include <QMap>
#include <QStringList>
#include <QRegularExpression>

class Translator
{
public:
    virtual ~Translator() = default;
    virtual QString translate(const QString &text) = 0;
};

class SimpleTranslator : public Translator
{
public:
    SimpleTranslator();
    QString translate(const QString &text) override;

protected:
    QMap<QString, QString> m_dict;
};

class OnlineTranslator : public Translator
{
public:
    OnlineTranslator();

    QString translate(const QString &text) override;

    void setServerUrl(const QString &url);
    void setApiKey(const QString &apiKey);
    void setSourceLanguage(const QString &source);
    void setTargetLanguage(const QString &target);

private:
    QString m_serverUrl = "http://127.0.0.1:5000/translate";
    QString m_apiKey;
    QString m_sourceLanguage = "en";
    QString m_targetLanguage = "ru";
};

class TechnicalTranslator : public Translator
{
public:
    TechnicalTranslator();

    QString translate(const QString &text) override;

private:
    OnlineTranslator m_onlineTranslator;

    QMap<QString, QString> m_russianTechnicalDict;

    QString prepareEnglishText(const QString &text) const;
    QString applyRussianTechnicalTerms(const QString &text) const;
};