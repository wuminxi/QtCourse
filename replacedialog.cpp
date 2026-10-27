#include "replacedialog.h"
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , m_textEdit(nullptr)
{
    setWindowTitle(tr("替换"));
    setMinimumWidth(380);

    QLabel *findLabel = new QLabel(tr("查找内容:"), this);
    QLabel *replaceLabel = new QLabel(tr("替换为:"), this);
    m_findEdit = new QLineEdit(this);
    m_replaceEdit = new QLineEdit(this);

    m_findNextBtn = new QPushButton(tr("查找下一个"), this);
    m_replaceBtn = new QPushButton(tr("替换"), this);
    m_replaceAllBtn = new QPushButton(tr("全部替换"), this);
    m_caseSensitiveCheck = new QCheckBox(tr("区分大小写"), this);
    m_statusLabel = new QLabel("", this);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(findLabel, 0, 0);
    gridLayout->addWidget(m_findEdit, 0, 1);
    gridLayout->addWidget(replaceLabel, 1, 0);
    gridLayout->addWidget(m_replaceEdit, 1, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_findNextBtn);
    btnLayout->addWidget(m_replaceBtn);
    btnLayout->addWidget(m_replaceAllBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(m_caseSensitiveCheck);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_statusLabel);

    connect(m_findNextBtn, &QPushButton::clicked, this, &ReplaceDialog::findNext);
    connect(m_replaceBtn, &QPushButton::clicked, this, &ReplaceDialog::replaceOne);
    connect(m_replaceAllBtn, &QPushButton::clicked, this, &ReplaceDialog::replaceAll);
}

void ReplaceDialog::setTextEdit(QPlainTextEdit *editor)
{
    m_textEdit = editor;
}

void ReplaceDialog::findNext()
{
    if (!m_textEdit) return;

    QString text = m_findEdit->text();
    if (text.isEmpty()) {
        m_statusLabel->setText(tr("请输入查找内容"));
        return;
    }

    QTextDocument::FindFlags flags;
    if (m_caseSensitiveCheck->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor cursor = m_textEdit->textCursor();
    QTextCursor found = m_textEdit->document()->find(text, cursor, flags);

    if (found.isNull()) {
        found = m_textEdit->document()->find(text, 0, flags);
        if (found.isNull()) {
            m_statusLabel->setText(tr("未找到"));
            return;
        }
    }

    m_textEdit->setTextCursor(found);
    m_statusLabel->setText(tr("已找到"));
}

void ReplaceDialog::replaceOne()
{
    if (!m_textEdit) return;

    QString findText = m_findEdit->text();
    QString replaceText = m_replaceEdit->text();

    if (findText.isEmpty()) {
        m_statusLabel->setText(tr("请输入查找内容"));
        return;
    }

    QTextCursor cursor = m_textEdit->textCursor();
    if (cursor.selectedText() == findText) {
        cursor.insertText(replaceText);
        m_statusLabel->setText(tr("已替换一处"));
    }
    findNext();
}

void ReplaceDialog::replaceAll()
{
    if (!m_textEdit) return;

    QString findText = m_findEdit->text();
    QString replaceText = m_replaceEdit->text();

    if (findText.isEmpty()) {
        m_statusLabel->setText(tr("请输入查找内容"));
        return;
    }

    QString content = m_textEdit->toPlainText();
    if (content.isEmpty()) {
        m_statusLabel->setText(tr("文档为空"));
        return;
    }

    // 先统计替换次数
    int count = 0;
    int idx = 0;
    Qt::CaseSensitivity cs = m_caseSensitiveCheck->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    while ((idx = content.indexOf(findText, idx, cs)) != -1) {
        count++;
        idx += findText.length();
    }

    // 执行替换
    QString newContent = content.replace(findText, replaceText, cs);
    m_textEdit->setPlainText(newContent);

    m_statusLabel->setText(tr("共替换 %1 处").arg(count));

    if (count == 0) {
        QMessageBox::information(this, tr("替换"), tr("未找到任何匹配内容。"));
    }
}
// 替换功能实现
