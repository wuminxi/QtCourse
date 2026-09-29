#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QtGlobal>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    clearClicked();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    const double operand = m_current.toDouble();

    if (m_operator.isEmpty() || m_waitingForOperand) {
        // 第一个操作数，或连续输入运算符：记录/替换运算符
        m_firstOperand = operand;
    } else {
        // 已有完整表达式：先结算前一步，支持连续运算
        m_firstOperand = calculate(m_firstOperand, operand, m_operator);
    }

    m_operator = op;
    m_waitingForOperand = true;
    m_current = formatNumber(m_firstOperand);
    updateDisplay();
}

void MainWindow::equalsClicked()
{
    if (m_operator.isEmpty())
        return;

    const double second = m_current.toDouble();
    m_firstOperand = calculate(m_firstOperand, second, m_operator);

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
    updateDisplay();
}

void MainWindow::clearEntryClicked()
{
    m_current = "0";
    m_waitingForOperand = true;
    updateDisplay();
}

void MainWindow::backspaceClicked()
{
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

double MainWindow::calculate(double a, double b, const QString &op)
{
    if (op == "+")
        return a + b;
    if (op == "-")
        return a - b;
    if (op == "×")
        return a * b;
    if (op == "÷")
        return a / b;
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

void MainWindow::updateDisplay()
{
    ui->display->setText(m_current);
}
