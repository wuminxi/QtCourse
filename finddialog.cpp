#include "finddialog.h"
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , m_textEdit(nullptr)
{
    setWindowTitle(tr("查找"));
    setMinimumWidth(350);

    QLabel *label = new QLabel(tr("查找内容:"), this);
    m_findEdit = new QLineEdit(this);

    m_findNextBtn = new QPushButton(tr("查找下一个"), this);
    m_findPrevBtn = new QPushButton(tr("查找上一个"), this);
    m_caseSensitiveCheck = new QCheckBox(tr("区分大小写"), this);
    m_statusLabel = new QLabel("", this);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(label);
    topLayout->addWidget(m_findEdit);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_findPrevBtn);
    btnLayout->addWidget(m_findNextBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_caseSensitiveCheck);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_statusLabel);

    connect(m_findNextBtn, &QPushButton::clicked, this, &FindDialog::findNext);
    connect(m_findPrevBtn, &QPushButton::clicked, this, &FindDialog::findPrev);
    connect(m_findEdit, &QLineEdit::textChanged, this, [this]() {
        m_statusLabel->setText("");
    });
}

void FindDialog::setTextEdit(QPlainTextEdit *editor)
{
    m_textEdit = editor;
}

void FindDialog::findNext()
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
        // 从头开始找
        found = m_textEdit->document()->find(text, 0, flags);
        if (found.isNull()) {
            m_statusLabel->setText(tr("未找到 \"%1\"").arg(text));
            QMessageBox::information(this, tr("查找"), tr("已到达文档末尾，未找到匹配内容。"));
            return;
        }
    }

    m_textEdit->setTextCursor(found);
    m_statusLabel->setText(tr("已找到"));
}

void FindDialog::findPrev()
{
    if (!m_textEdit) return;

    QString text = m_findEdit->text();
    if (text.isEmpty()) {
        m_statusLabel->setText(tr("请输入查找内容"));
        return;
    }

    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    if (m_caseSensitiveCheck->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor cursor = m_textEdit->textCursor();
    QTextCursor found = m_textEdit->document()->find(text, cursor, flags);

    if (found.isNull()) {
        found = m_textEdit->document()->find(text, m_textEdit->document()->characterCount() - 1, flags);
        if (found.isNull()) {
            m_statusLabel->setText(tr("未找到 \"%1\"").arg(text));
            return;
        }
    }

    m_textEdit->setTextCursor(found);
    m_statusLabel->setText(tr("已找到"));
}
// 查找对话框
