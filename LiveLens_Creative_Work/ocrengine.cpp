#include "ocrengine.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDebug>
#include <QImage>
#include <QDir>
#include <QFile>

OCREngine* OCREngine::m_instance = nullptr;

OCREngine::OCREngine(QObject *parent) : QObject(parent)
{
    m_tessApi = new tesseract::TessBaseAPI();
}

OCREngine::~OCREngine()
{
    auto *api = static_cast<tesseract::TessBaseAPI*>(m_tessApi);
    api->End();
    delete api;
}

void OCREngine::initialize(const QString &tessdataPath)
{
    if (m_instance) {
        qDebug() << "Already initialized";
        return;
    }

    m_instance = new OCREngine();
    auto *api = static_cast<tesseract::TessBaseAPI*>(m_instance->m_tessApi);

    if (api->Init(tessdataPath.toLocal8Bit().constData(), "eng")) {
        qFatal("Tesseract init failed");
    }

    qDebug() << "Tesseract initialized";
}

OCREngine *OCREngine::instance()
{
    if (!m_instance) {
        qFatal("Call initialize() first!");
    }
    return m_instance;
}

QString OCREngine::recognize(const QImage &image)
{
    if (!m_instance || image.isNull()) return QString();

    auto *api = static_cast<tesseract::TessBaseAPI*>(m_instance->m_tessApi);

    QString tempPath = QDir::temp().absoluteFilePath("ocr_temp.png");
    if (!image.save(tempPath)) return QString();

    Pix *pix = pixRead(tempPath.toLocal8Bit().constData());
    if (!pix) {
        QFile::remove(tempPath);
        return QString();
    }

    api->SetImage(pix);
    char *outText = api->GetUTF8Text();
    QString result = QString::fromUtf8(outText);

    //delete[] outText;
    pixDestroy(&pix);
    QFile::remove(tempPath);

    return result.trimmed();
}

QString OCREngine::recognizeWord(const QImage &image)
{
    if (!m_instance || image.isNull()) return QString();

    auto *api = static_cast<tesseract::TessBaseAPI*>(m_instance->m_tessApi);

    QString tempPath = QDir::temp().absoluteFilePath("ocr_word_temp.png");
    if (!image.save(tempPath)) return QString();

    Pix *pix = pixRead(tempPath.toLocal8Bit().constData());
    if (!pix) {
        QFile::remove(tempPath);
        return QString();
    }

    api->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
    api->SetImage(pix);

    char *outText = api->GetUTF8Text();
    QString result = QString::fromUtf8(outText);

    //delete[] outText;
    pixDestroy(&pix);
    api->SetPageSegMode(tesseract::PSM_AUTO);
    QFile::remove(tempPath);

    return result.trimmed();
}