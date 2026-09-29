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
    void operatorClicked();
    void equalsClicked();
    void clearClicked();
    void clearEntryClicked();
    void backspaceClicked();

private:
    void inputDigit(const QString &digit);
    void applyOperator(const QString &op);
    double calculate(double a, double b, const QString &op);
    QString formatNumber(double value) const;
    void updateDisplay();

    Ui::MainWindow *ui;
    QString m_current;               // 当前输入/显示的数字
    double m_firstOperand = 0.0;     // 第一操作数
    QString m_operator;              // 当前运算符（+ - × ÷）
    bool m_waitingForOperand = true; // 等待输入下一操作数
};
#endif // MAINWINDOW_H
