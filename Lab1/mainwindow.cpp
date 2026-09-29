#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QtGlobal>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 显示框只读且不抢占焦点，保证键盘事件统一由主窗口处理
    ui->display->setReadOnly(true);
    ui->display->setFocusPolicy(Qt::NoFocus);

    // 所有按钮不抢占焦点，便于鼠标与键盘输入走同一套逻辑
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *b : buttons)
        b->setFocusPolicy(Qt::NoFocus);

    // 数字键复用同一槽函数，通过 sender()->text() 获取数字
    for (int i = 0; i <= 9; ++i) {
        QPushButton *btn = findChild<QPushButton *>(QString("btn%1").arg(i));
        connect(btn, &QPushButton::clicked, this, &MainWindow::digitClicked);
    }
    connect(ui->btnDot, &QPushButton::clicked, this, &MainWindow::decimalClicked);
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::operatorClicked);
    connect(ui->btnSub, &QPushButton::clicked, this, &MainWindow::operatorClicked);
    connect(ui->btnMul, &QPushButton::clicked, this, &MainWindow::operatorClicked);
    connect(ui->btnDiv, &QPushButton::clicked, this, &MainWindow::operatorClicked);
    connect(ui->btnEquals, &QPushButton::clicked, this, &MainWindow::equalsClicked);
    connect(ui->btnC, &QPushButton::clicked, this, &MainWindow::clearClicked);
    connect(ui->btnCE, &QPushButton::clicked, this, &MainWindow::clearEntryClicked);
    connect(ui->btnBackspace, &QPushButton::clicked, this, &MainWindow::backspaceClicked);
    connect(ui->btnSign, &QPushButton::clicked, this, &MainWindow::signClicked);
    connect(ui->btnPercent, &QPushButton::clicked, this, &MainWindow::percentClicked);
    connect(ui->btnReciprocal, &QPushButton::clicked, this, &MainWindow::reciprocalClicked);
    connect(ui->btnSquare, &QPushButton::clicked, this, &MainWindow::squareClicked);
    connect(ui->btnSqrt, &QPushButton::clicked, this, &MainWindow::sqrtClicked);

    clearClicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ---------- 键盘事件：与鼠标按键共用同一套处理逻辑 ----------
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        inputDigit(QString::number(key - Qt::Key_0));
    } else {
        switch (key) {
        case Qt::Key_Period:
        case Qt::Key_Comma:
            decimalClicked();
            break;
        case Qt::Key_Plus:
            applyOperator("+");
            break;
        case Qt::Key_Minus:
            applyOperator("-");
            break;
        case Qt::Key_Asterisk:
            applyOperator("×");
            break;
        case Qt::Key_Slash:
            applyOperator("÷");
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Equal:
            equalsClicked();
            break;
        case Qt::Key_Backspace:
            backspaceClicked();
            break;
        case Qt::Key_Delete:
            clearEntryClicked();
            break;
        case Qt::Key_Escape:
            clearClicked();
            break;
        case Qt::Key_Percent:
            percentClicked();
            break;
        default:
            QMainWindow::keyPressEvent(event);
            return;
        }
    }
    event->accept();
}

void MainWindow::digitClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;
    inputDigit(btn->text());
}

void MainWindow::inputDigit(const QString &digit)
{
    if (m_error)
        return; // 出错后需先按 C 清除

    if (m_waitingForOperand) {
        m_current = digit;            // 开启新操作数
        m_waitingForOperand = false;
    } else if (m_current == "0" || m_current == "-0") {
        m_current = digit;            // 去掉前导零
    } else {
        m_current += digit;
    }
    updateDisplay();
}

void MainWindow::decimalClicked()
{
    if (m_error)
        return;

    if (m_waitingForOperand) {
        m_current = "0.";
        m_waitingForOperand = false;
    } else if (!m_current.contains('.')) {
        m_current += '.';            // 已含小数点则忽略，避免重复输入
    }
    updateDisplay();
}

void MainWindow::operatorClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;
    applyOperator(btn->text());
}

void MainWindow::applyOperator(const QString &op)
{
    if (m_error)
        return;

    const double operand = m_current.toDouble();

    if (m_operator.isEmpty() || m_waitingForOperand) {
        // 第一个操作数，或连续输入运算符：用新运算符替换旧运算符
        m_firstOperand = operand;
    } else {
        // 已有完整表达式：先结算前一步，支持连续运算
        m_firstOperand = calculate(m_firstOperand, operand, m_operator);
        if (m_error)
            return;
    }

    m_operator = op;
    m_waitingForOperand = true;
    m_current = formatNumber(m_firstOperand);
    updateDisplay();
}

void MainWindow::equalsClicked()
{
    if (m_error || m_operator.isEmpty())
        return;

    const double second = m_current.toDouble();
    m_firstOperand = calculate(m_firstOperand, second, m_operator);
    if (m_error)
        return;

    m_current = formatNumber(m_firstOperand);
    m_operator.clear();
    m_waitingForOperand = true;      // 计算结果后继续输入将开启新操作数
    updateDisplay();
}

void MainWindow::clearClicked()
{
    m_current = "0";
    m_firstOperand = 0.0;
    m_operator.clear();
    m_waitingForOperand = true;
    m_error = false;
    updateDisplay();
}

void MainWindow::clearEntryClicked()
{
    if (m_error) {
        clearClicked();              // 出错时 CE 也用于清除错误状态
        return;
    }
    m_current = "0";
    m_waitingForOperand = true;
    updateDisplay();
}

void MainWindow::backspaceClicked()
{
    if (m_error)
        return;
    if (m_waitingForOperand)
        return;

    if (m_current.length() > 1) {
        m_current.chop(1);
        if (m_current.isEmpty() || m_current == "-")
            m_current = "0";
    } else {
        m_current = "0";
    }
    updateDisplay();
}

void MainWindow::signClicked()
{
    if (m_error)
        return;
    if (m_current == "0" || m_current == "0.")
        return;
    if (m_current.startsWith('-'))
        m_current.remove(0, 1);
    else
        m_current.prepend('-');
    updateDisplay();
}

void MainWindow::percentClicked()
{
    if (m_error)
        return;
    m_current = formatNumber(m_current.toDouble() / 100.0);
    updateDisplay();
}

void MainWindow::reciprocalClicked()
{
    if (m_error)
        return;
    const double v = m_current.toDouble();
    if (qFuzzyIsNull(v)) {
        showError("除数不能为 0");
        return;
    }
    m_current = formatNumber(1.0 / v);
    updateDisplay();
}

void MainWindow::squareClicked()
{
    if (m_error)
        return;
    const double v = m_current.toDouble();
    m_current = formatNumber(v * v);
    updateDisplay();
}

void MainWindow::sqrtClicked()
{
    if (m_error)
        return;
    const double v = m_current.toDouble();
    if (v < 0) {
        showError("负数不能开方");
        return;
    }
    m_current = formatNumber(std::sqrt(v));
    updateDisplay();
}

double MainWindow::calculate(double a, double b, const QString &op)
{
    if (op == "+")
        return a + b;
    if (op == "-")
        return a - b;
    if (op == "×")
        return a * b;
    if (op == "÷") {
        if (qFuzzyIsNull(b)) {
            showError("除数不能为 0");
            return 0.0;
        }
        return a / b;
    }
    return b;
}

QString MainWindow::formatNumber(double value) const
{
    if (qFuzzyIsNull(value))
        return "0";
    QString s = QString::number(value, 'f', 10);
    while (s.contains('.') && s.endsWith('0'))
        s.chop(1);
    if (s.endsWith('.'))
        s.chop(1);
    if (s == "-0")
        s = "0";
    return s;
}

void MainWindow::showError(const QString &msg)
{
    m_error = true;
    m_current = msg;
    updateDisplay();
}

void MainWindow::updateDisplay()
{
    ui->display->setText(m_current);
}
