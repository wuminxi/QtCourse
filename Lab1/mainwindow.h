#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    // 键盘事件：与鼠标按键处理逻辑保持一致
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void digitClicked();
    void decimalClicked();
    void operatorClicked();
    void equalsClicked();
    void clearClicked();
    void clearEntryClicked();
    void backspaceClicked();
    void signClicked();
    void percentClicked();
    void reciprocalClicked();
    void squareClicked();
    void sqrtClicked();

private:
    void inputDigit(const QString &digit);
    void applyOperator(const QString &op);
    double calculate(double a, double b, const QString &op);
    QString formatNumber(double value) const;
    void showError(const QString &msg);
    void updateDisplay();

    Ui::MainWindow *ui;
    QString m_current;               // 当前输入/显示的数字
    double m_firstOperand = 0.0;     // 第一操作数
    QString m_operator;              // 当前运算符（+ - × ÷）
    bool m_waitingForOperand = true; // 等待输入下一操作数
    bool m_error = false;            // 错误状态标志（除零等）
};
#endif // MAINWINDOW_H
