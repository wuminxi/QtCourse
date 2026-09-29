#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void digitClicked();
    void decimalClicked();
    void clearClicked();
    void clearEntryClicked();
    void backspaceClicked();

private:
    void inputDigit(const QString &digit);
    void updateDisplay();

    Ui::MainWindow *ui;
    QString m_current;               // 当前输入/显示的数字
    bool m_waitingForOperand = true; // 等待输入下一操作数
};
#endif // MAINWINDOW_H
