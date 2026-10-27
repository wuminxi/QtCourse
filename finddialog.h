#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTextCursor>

class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;
class QPlainTextEdit;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    void setTextEdit(QPlainTextEdit *editor);

private slots:
    void findNext();
    void findPrev();

private:
    QLineEdit *m_findEdit;
    QPushButton *m_findNextBtn;
    QPushButton *m_findPrevBtn;
    QCheckBox *m_caseSensitiveCheck;
    QLabel *m_statusLabel;
    QPlainTextEdit *m_textEdit;
    QTextCursor m_lastCursor;
};

#endif // FINDDIALOG_H
