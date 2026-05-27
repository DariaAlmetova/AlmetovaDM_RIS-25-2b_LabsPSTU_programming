#pragma once
#include <QObject>
#include <QString>
#include <QImage>
#include <tesseract/baseapi.h>

class OCREngine : public QObject
{
    Q_OBJECT
public:
    explicit OCREngine(QObject *parent = nullptr);
    ~OCREngine();
    static void initialize(const QString &tessdataPath);
    static OCREngine *instance();

    QString recognize(const QImage &image);
    QString recognizeWord(const QImage &image);

private:
    void *m_tessApi = nullptr;
    static OCREngine* m_instance;
};