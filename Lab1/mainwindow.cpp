#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QLineEdit>
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

    // 按功能为按钮分组设置角色属性，配合样式表区分配色
    // 数字键（0~9 与小数点）
    for (int i = 0; i <= 9; ++i)
        findChild<QPushButton *>(QString("btn%1").arg(i))->setProperty("role", "digit");
    ui->btnDot->setProperty("role", "digit");
    // 运算符键
    ui->btnAdd->setProperty("role", "operator");
    ui->btnSub->setProperty("role", "operator");
    ui->btnMul->setProperty("role", "operator");
    ui->btnDiv->setProperty("role", "operator");
    // 等号键
    ui->btnEquals->setProperty("role", "equals");
    // 功能键（清除、退格、百分号等）
    ui->btnC->setProperty("role", "function");
    ui->btnCE->setProperty("role", "function");
    ui->btnBackspace->setProperty("role", "function");
    ui->btnPercent->setProperty("role", "function");
    ui->btnSign->setProperty("role", "function");
    ui->btnReciprocal->setProperty("role", "function");
    ui->btnSquare->setProperty("role", "function");
    ui->btnSqrt->setProperty("role", "function");

    // 样式表：深色主题，数字/运算符/功能键三色区分，等号高亮
    setStyleSheet(QStringLiteral(
        "QMainWindow { background-color: #2b2b2b; }"
        "QLineEdit#display {"
        "  background-color: #2b2b2b; color: #ffffff; border: none;"
        "  font-size: 28px; font-weight: bold; padding: 6px 12px;"
        "  selection-background-color: #555555;"
        "}"
        "QPushButton { border: none; border-radius: 14px; font-size: 16px; }"
        "QPushButton[role=\"digit\"] { background-color: #4a4a4a; color: #ffffff; }"
        "QPushButton[role=\"digit\"]:hover   { background-color: #5a5a5a; }"
        "QPushButton[role=\"digit\"]:pressed { background-color: #3a3a3a; }"
        "QPushButton[role=\"function\"] { background-color: #3a3a3a; color: #cccccc; }"
        "QPushButton[role=\"function\"]:hover   { background-color: #4a4a4a; }"
        "QPushButton[role=\"operator\"] { background-color: #ff9f0a; color: #ffffff; }"
        "QPushButton[role=\"operator\"]:hover   { background-color: #ffb340; }"
        "QPushButton[role=\"operator\"]:pressed { background-color: #e08a00; }"
        "QPushButton[role=\"equals\"] { background-color: #ff9f0a; color: #ffffff; }"
        "QPushButton[role=\"equals\"]:hover { background-color: #ffb340; }"
    ));

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

    // 初始状态
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

// ---------- 数字输入 ----------
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

// ---------- 小数点 ----------
void MainWindow::decimalClicked()
{
    if (m_error)
        return;

    if (m_waitingForOperand) {
        m_current = "0.";
        m_waitingForOperand = false;
    } else if (!m_current.contains('.')) {
        m_current += '.';
    }
    // 若当前操作数已含小数点，直接忽略，避免重复输入小数点
    updateDisplay();
}

// ---------- 运算符 ----------
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
        // 输入第一个操作数，或连续输入运算符：用新运算符替换旧运算符
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

// ---------- 等号 ----------
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
    m_waitingForOperand = true; // 计算结果后继续输入将开启新操作数
    updateDisplay();
}

// ---------- 清除 ----------
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
        clearClicked(); // 出错时 CE 也用于清除错误状态
        return;
    }
    m_current = "0";
    m_waitingForOperand = true;
    updateDisplay();
}

// ---------- 退格 ----------
void MainWindow::backspaceClicked()
{
    if (m_error)
        return;
    if (m_waitingForOperand)
        return; // 刚按过运算符，当前无数字可退

    if (m_current.length() > 1) {
        m_current.chop(1);
        // 退格后若只剩负号或空串，回退为 0
        if (m_current.isEmpty() || m_current == "-")
            m_current = "0";
    } else {
        m_current = "0";
    }
    updateDisplay();
}

// ---------- 正负号 ----------
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

// ---------- 百分号 ----------
void MainWindow::percentClicked()
{
    if (m_error)
        return;
    m_current = formatNumber(m_current.toDouble() / 100.0);
    updateDisplay();
}

// ---------- 倒数 ----------
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

// ---------- 平方 ----------
void MainWindow::squareClicked()
{
    if (m_error)
        return;
    const double v = m_current.toDouble();
    m_current = formatNumber(v * v);
    updateDisplay();
}

// ---------- 开方 ----------
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

// ---------- 四则运算核心 ----------
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

// ---------- 工具函数 ----------
QString MainWindow::formatNumber(double value) const
{
    if (qFuzzyIsNull(value))
        return "0";
    QString s = QString::number(value, 'f', 10);
    // 去掉小数末尾多余的 0 和末尾小数点
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
