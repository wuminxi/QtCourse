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
    // 键盘事件：统一处理键盘输入，与鼠标按键逻辑保持一致
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // 数字键 0~9
    void digitClicked();
    // 小数点
    void decimalClicked();
    // 运算符 + - × ÷
    void operatorClicked();
    // 等号 =
    void equalsClicked();
    // 清除全部 C
    void clearClicked();
    // 清除当前输入 CE
    void clearEntryClicked();
    // 退格 ⌫
    void backspaceClicked();
    // 正负号 ±
    void signClicked();
    // 百分号 %
    void percentClicked();
    // 倒数 1/x
    void reciprocalClicked();
    // 平方 x^2
    void squareClicked();
    // 开方 √
    void sqrtClicked();

private:
    // 鼠标与键盘共用的核心逻辑
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
    bool m_error = false;            // 错误状态（除零、负数开方等）
};
#endif // MAINWINDOW_H
