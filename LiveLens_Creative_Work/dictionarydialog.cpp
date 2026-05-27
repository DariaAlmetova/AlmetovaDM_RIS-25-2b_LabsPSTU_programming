#include "dictionarydialog.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

DictionaryDialog::DictionaryDialog(QMap<QString, QString> *dictionary,
                                   const QString &title,
                                   QWidget *parent)
    : QDialog(parent),
    m_dictionary(dictionary)
{
    setWindowTitle(title);
    resize(520, 380);

    setupUi();
    loadTable();
}

void DictionaryDialog::setupUi()
{
    m_table = new QTableWidget(this);
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({"Английское слово", "Перевод на русский"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    m_addBtn = new QPushButton("Добавить слово", this);
    m_deleteBtn = new QPushButton("Удалить слово", this);
    m_clearBtn = new QPushButton("Очистить словарь", this);
    m_closeBtn = new QPushButton("Закрыть", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_addBtn);
    buttonLayout->addWidget(m_deleteBtn);
    buttonLayout->addWidget(m_clearBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_table);
    mainLayout->addLayout(buttonLayout);

    connect(m_addBtn, &QPushButton::clicked, this, &DictionaryDialog::addWord);
    connect(m_deleteBtn, &QPushButton::clicked, this, &DictionaryDialog::deleteWord);
    connect(m_clearBtn, &QPushButton::clicked, this, &DictionaryDialog::clearDictionary);
    connect(m_closeBtn, &QPushButton::clicked, this, &DictionaryDialog::accept);

    connect(m_table, &QTableWidget::cellChanged, this, [this](int, int) {
        saveTable();
    });
}

void DictionaryDialog::loadTable()
{
    if (!m_dictionary)
        return;

    m_table->blockSignals(true);
    m_table->setRowCount(0);

    int row = 0;

    for (auto it = m_dictionary->begin(); it != m_dictionary->end(); ++it) {
        m_table->insertRow(row);

        QTableWidgetItem *wordItem = new QTableWidgetItem(it.key());
        QTableWidgetItem *translationItem = new QTableWidgetItem(it.value());

        m_table->setItem(row, 0, wordItem);
        m_table->setItem(row, 1, translationItem);

        row++;
    }

    m_table->blockSignals(false);
}

void DictionaryDialog::saveTable()
{
    if (!m_dictionary)
        return;

    m_dictionary->clear();

    for (int row = 0; row < m_table->rowCount(); ++row) {
        QTableWidgetItem *wordItem = m_table->item(row, 0);
        QTableWidgetItem *translationItem = m_table->item(row, 1);

        if (!wordItem || !translationItem)
            continue;

        QString word = wordItem->text().trimmed().toLower();
        QString translation = translationItem->text().trimmed();

        if (word.isEmpty() || translation.isEmpty())
            continue;

        m_dictionary->insert(word, translation);
    }
}

void DictionaryDialog::addWord()
{
    bool ok = false;

    QString word = QInputDialog::getText(
        this,
        "Добавить слово",
        "Введите английское слово:",
        QLineEdit::Normal,
        "",
        &ok
        );

    if (!ok || word.trimmed().isEmpty())
        return;

    word = word.trimmed().toLower();

    QString autoTranslation = autoTranslateWord(word);

    if (autoTranslation.isEmpty()) {
        autoTranslation = "";
        QMessageBox::warning(
            this,
            "Автоперевод",
            "Не удалось автоматически перевести слово.\n"
            "Проверьте, что LibreTranslate запущен."
            );
    }

    QString translation = QInputDialog::getText(
        this,
        "Перевод слова",
        QString("Введите перевод для слова \"%1\":").arg(word),
        QLineEdit::Normal,
        autoTranslation,
        &ok
        );

    if (!ok || translation.trimmed().isEmpty())
        return;

    translation = translation.trimmed();

    (*m_dictionary)[word] = translation;

    loadTable();
}

void DictionaryDialog::deleteWord()
{
    int row = m_table->currentRow();

    if (row < 0) {
        QMessageBox::warning(this, "Удаление", "Выберите слово для удаления.");
        return;
    }

    QTableWidgetItem *wordItem = m_table->item(row, 0);

    if (!wordItem)
        return;

    QString word = wordItem->text().trimmed().toLower();

    if (QMessageBox::question(
            this,
            "Удаление слова",
            QString("Удалить слово \"%1\" из словаря?").arg(word)
            ) != QMessageBox::Yes) {
        return;
    }

    m_dictionary->remove(word);
    loadTable();
}

void DictionaryDialog::clearDictionary()
{
    if (!m_dictionary)
        return;

    if (m_dictionary->isEmpty()) {
        QMessageBox::information(this, "Словарь", "Словарь уже пуст.");
        return;
    }

    if (QMessageBox::question(
            this,
            "Очистка словаря",
            "Вы действительно хотите очистить весь словарь?"
            ) != QMessageBox::Yes) {
        return;
    }

    m_dictionary->clear();
    loadTable();
}

QString DictionaryDialog::autoTranslateWord(const QString &word)
{
    QString cleanWord = word.trimmed();

    if (cleanWord.isEmpty())
        return "";

    QString translated = m_onlineTranslator.translate(cleanWord);

    if (translated.startsWith("Ошибка"))
        return "";

    return translated.trimmed();
}