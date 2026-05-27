#ifndef DICTIONARYDIALOG_H
#define DICTIONARYDIALOG_H

#include <QDialog>
#include <QMap>
#include "translator.h"

class QTableWidget;
class QPushButton;

class DictionaryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DictionaryDialog(QMap<QString, QString> *dictionary, const QString &title, QWidget *parent = nullptr);

private slots:
    void addWord();
    void deleteWord();
    void clearDictionary();

private:
    void setupUi();
    void loadTable();
    void saveTable();

    QMap<QString, QString> *m_dictionary = nullptr;

    QTableWidget *m_table = nullptr;
    QPushButton *m_addBtn = nullptr;
    QPushButton *m_deleteBtn = nullptr;
    QPushButton *m_clearBtn = nullptr;
    QPushButton *m_closeBtn = nullptr;

    QString autoTranslateWord(const QString &word);
    OnlineTranslator m_onlineTranslator;
};

#endif // DICTIONARYDIALOG_H