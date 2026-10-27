#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>
#include <QTextCursor>

class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;
class QPlainTextEdit;

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    void setTextEdit(QPlainTextEdit *editor);

private slots:
    void findNext();
    void replaceOne();
    void replaceAll();

private:
    QLineEdit *m_findEdit;
    QLineEdit *m_replaceEdit;
    QPushButton *m_findNextBtn;
    QPushButton *m_replaceBtn;
    QPushButton *m_replaceAllBtn;
    QCheckBox *m_caseSensitiveCheck;
    QLabel *m_statusLabel;
    QPlainTextEdit *m_textEdit;
};

#endif // REPLACEDIALOG_H
