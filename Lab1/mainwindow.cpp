#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>

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

void MainWindow::clearClicked()
{
    m_current = "0";
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

void MainWindow::updateDisplay()
{
    ui->display->setText(m_current);
}
