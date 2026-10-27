#include "mainwindow.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "aboutdialog.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QFontDialog>
#include <QColorDialog>
#include <QCloseEvent>
#include <QTextBlock>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_findDlg(nullptr)
    , m_replaceDlg(nullptr)
    , m_isModified(false)
{
    m_textEdit = new CodeEditor(this);
    setCentralWidget(m_textEdit);

    m_findDlg = new FindDialog(this);
    m_replaceDlg = new ReplaceDialog(this);
    m_findDlg->setTextEdit(m_textEdit);
    m_replaceDlg->setTextEdit(m_textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    connect(m_textEdit, &CodeEditor::documentChanged,
            this, &MainWindow::onTextChanged);
    connect(m_textEdit, &CodeEditor::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);
    connect(m_textEdit, &CodeEditor::copyAvailable,
            this, &MainWindow::updateEditActions);
    connect(m_textEdit, &CodeEditor::undoAvailable,
            m_undoAct, &QAction::setEnabled);
    connect(m_textEdit, &CodeEditor::redoAvailable,
            m_redoAct, &QAction::setEnabled);

    // 自动保存定时器（30秒）
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSave);
    m_autoSaveTimer->start(30000);

    setCurrentFile(QString());
    resize(800, 600);
    updateEditActions();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    // 文件Action
    m_newAct = new QAction(QIcon(":/images/new.png"), tr("新建(&N)"), this);
    m_newAct->setShortcut(QKeySequence::New);
    connect(m_newAct, &QAction::triggered, this, &MainWindow::newFile);

    m_openAct = new QAction(QIcon(":/images/open.png"), tr("打开(&O)..."), this);
    m_openAct->setShortcut(QKeySequence::Open);
    connect(m_openAct, &QAction::triggered, this, &MainWindow::openFile);

    m_saveAct = new QAction(QIcon(":/images/save.png"), tr("保存(&S)"), this);
    m_saveAct->setShortcut(QKeySequence::Save);
    connect(m_saveAct, &QAction::triggered, this, [this]() { saveFile(); });

    m_saveAsAct = new QAction(tr("另存为(&A)..."), this);
    connect(m_saveAsAct, &QAction::triggered, this, &MainWindow::saveAsFile);

    m_exitAct = new QAction(tr("退出(&X)"), this);
    m_exitAct->setShortcut(tr("Ctrl+Q"));
    connect(m_exitAct, &QAction::triggered, this, &QMainWindow::close);

    // 编辑Action
    m_undoAct = new QAction(QIcon(":/images/undo.png"), tr("撤销(&U)"), this);
    m_undoAct->setShortcut(QKeySequence::Undo);
    m_undoAct->setEnabled(false);
    connect(m_undoAct, &QAction::triggered, this, &MainWindow::undo);

    m_redoAct = new QAction(QIcon(":/images/redo.png"), tr("重做(&R)"), this);
    m_redoAct->setShortcut(QKeySequence::Redo);
    m_redoAct->setEnabled(false);
    connect(m_redoAct, &QAction::triggered, this, &MainWindow::redo);

    m_cutAct = new QAction(QIcon(":/images/cut.png"), tr("剪切(&T)"), this);
    m_cutAct->setShortcut(QKeySequence::Cut);
    m_cutAct->setEnabled(false);
    connect(m_cutAct, &QAction::triggered, this, &MainWindow::cut);

    m_copyAct = new QAction(QIcon(":/images/copy.png"), tr("复制(&C)"), this);
    m_copyAct->setShortcut(QKeySequence::Copy);
    m_copyAct->setEnabled(false);
    connect(m_copyAct, &QAction::triggered, this, &MainWindow::copy);

    m_pasteAct = new QAction(QIcon(":/images/paste.png"), tr("粘贴(&P)"), this);
    m_pasteAct->setShortcut(QKeySequence::Paste);
    connect(m_pasteAct, &QAction::triggered, this, &MainWindow::paste);

    m_selectAllAct = new QAction(tr("全选(&A)"), this);
    m_selectAllAct->setShortcut(QKeySequence::SelectAll);
    connect(m_selectAllAct, &QAction::triggered, this, &MainWindow::selectAll);

    m_findAct = new QAction(tr("查找(&F)..."), this);
    m_findAct->setShortcut(QKeySequence::Find);
    connect(m_findAct, &QAction::triggered, this, &MainWindow::showFind);

    m_replaceAct = new QAction(tr("替换(&E)..."), this);
    m_replaceAct->setShortcut(tr("Ctrl+H"));
    connect(m_replaceAct, &QAction::triggered, this, &MainWindow::showReplace);

    // 格式Action
    m_fontAct = new QAction(tr("字体..."), this);
    connect(m_fontAct, &QAction::triggered, this, &MainWindow::setFont);

    m_fontColorAct = new QAction(tr("字体颜色..."), this);
    connect(m_fontColorAct, &QAction::triggered, this, &MainWindow::setFontColor);

    m_bgColorAct = new QAction(tr("背景颜色..."), this);
    connect(m_bgColorAct, &QAction::triggered, this, &MainWindow::setBackgroundColor);

    m_wrapAct = new QAction(tr("自动换行"), this);
    m_wrapAct->setCheckable(true);
    m_wrapAct->setChecked(true);
    connect(m_wrapAct, &QAction::triggered, this, &MainWindow::toggleWrap);

    m_lineNumberAct = new QAction(tr("显示行号"), this);
    m_lineNumberAct->setCheckable(true);
    m_lineNumberAct->setChecked(true);
    connect(m_lineNumberAct, &QAction::triggered, this, &MainWindow::toggleLineNumber);

    // 视图Action
    m_toolbarAct = new QAction(tr("工具栏"), this);
    m_toolbarAct->setCheckable(true);
    m_toolbarAct->setChecked(true);
    connect(m_toolbarAct, &QAction::triggered, this, &MainWindow::toggleToolbar);

    m_statusbarAct = new QAction(tr("状态栏"), this);
    m_statusbarAct->setCheckable(true);
    m_statusbarAct->setChecked(true);
    connect(m_statusbarAct, &QAction::triggered, this, &MainWindow::toggleStatusBar);

    // 帮助Action
    m_aboutAct = new QAction(tr("关于(&A)"), this);
    connect(m_aboutAct, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    m_fileMenu->addAction(m_newAct);
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_saveAct);
    m_fileMenu->addAction(m_saveAsAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);

    m_editMenu = menuBar()->addMenu(tr("编辑(&E)"));
    m_editMenu->addAction(m_undoAct);
    m_editMenu->addAction(m_redoAct);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_cutAct);
    m_editMenu->addAction(m_copyAct);
    m_editMenu->addAction(m_pasteAct);
    m_editMenu->addAction(m_selectAllAct);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_findAct);
    m_editMenu->addAction(m_replaceAct);

    m_formatMenu = menuBar()->addMenu(tr("格式(&O)"));
    m_formatMenu->addAction(m_fontAct);
    m_formatMenu->addAction(m_fontColorAct);
    m_formatMenu->addAction(m_bgColorAct);
    m_formatMenu->addSeparator();
    m_formatMenu->addAction(m_wrapAct);
    m_formatMenu->addAction(m_lineNumberAct);

    m_viewMenu = menuBar()->addMenu(tr("视图(&V)"));
    m_viewMenu->addAction(m_toolbarAct);
    m_viewMenu->addAction(m_statusbarAct);

    m_helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    m_helpMenu->addAction(m_aboutAct);
}

void MainWindow::createToolBars()
{
    m_fileToolBar = addToolBar(tr("文件"));
    m_fileToolBar->setObjectName("fileToolBar");
    m_fileToolBar->addAction(m_newAct);
    m_fileToolBar->addAction(m_openAct);
    m_fileToolBar->addAction(m_saveAct);

    m_editToolBar = addToolBar(tr("编辑"));
    m_editToolBar->setObjectName("editToolBar");
    m_editToolBar->addAction(m_undoAct);
    m_editToolBar->addAction(m_redoAct);
    m_editToolBar->addSeparator();
    m_editToolBar->addAction(m_cutAct);
    m_editToolBar->addAction(m_copyAct);
    m_editToolBar->addAction(m_pasteAct);

    m_formatToolBar = addToolBar(tr("格式"));
    m_formatToolBar->setObjectName("formatToolBar");
    m_formatToolBar->addAction(m_fontAct);
    m_formatToolBar->addAction(m_fontColorAct);
    m_formatToolBar->addAction(m_bgColorAct);
}

void MainWindow::createStatusBar()
{
    m_positionLabel = new QLabel(tr("行: 1, 列: 1"));
    m_positionLabel->setMinimumSize(120, 20);
    statusBar()->addWidget(m_positionLabel);

    m_lengthLabel = new QLabel(tr("字符数: 0"));
    m_lengthLabel->setMinimumSize(120, 20);
    statusBar()->addPermanentWidget(m_lengthLabel);

    statusBar()->showMessage(tr("就绪"));
}

void MainWindow::createDockWidgets()
{
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        m_textEdit->clear();
        setCurrentFile(QString());
        statusBar()->showMessage(tr("已新建文件"), 2000);
    }
}

void MainWindow::openFile()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("打开文件"), "", tr("文本文件 (*.txt *.cpp *.h *.ui *.md);;所有文件 (*.*)"));
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

void MainWindow::saveFile()
{
    if (m_currentFile.isEmpty()) {
        return saveAsFile();
    } else {
        saveFile(m_currentFile);
    }
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("保存"),
                             tr("无法保存文件 %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << m_textEdit->toPlainText();
    file.close();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("文件已保存"), 2000);
    m_isModified = false;
    documentModified(false);
    return true;
}

void MainWindow::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("另存为"), m_currentFile.isEmpty() ? "未命名.txt" : m_currentFile,
        tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (fileName.isEmpty())
        return;

    saveFile(fileName);
}

void MainWindow::undo()
{
    m_textEdit->undo();
}

void MainWindow::redo()
{
    m_textEdit->redo();
}

void MainWindow::cut()
{
    m_textEdit->cut();
}

void MainWindow::copy()
{
    m_textEdit->copy();
}

void MainWindow::paste()
{
    m_textEdit->paste();
}

void MainWindow::selectAll()
{
    m_textEdit->selectAll();
}

void MainWindow::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_textEdit->font(), this, tr("选择字体"));
    if (ok) {
        m_textEdit->setFont(font);
    }
}

void MainWindow::setFontColor()
{
    QColor color = QColorDialog::getColor(Qt::black, this, tr("选择字体颜色"));
    if (color.isValid()) {
        m_textEdit->setStyleSheet(QString("QPlainTextEdit { color: %1; }").arg(color.name()));
    }
}

void MainWindow::setBackgroundColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, tr("选择背景颜色"));
    if (color.isValid()) {
        m_textEdit->setStyleSheet(
            QString("QPlainTextEdit { background-color: %1; }").arg(color.name()));
    }
}

void MainWindow::toggleWrap()
{
    if (m_wrapAct->isChecked()) {
        m_textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    } else {
        m_textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void MainWindow::toggleLineNumber(bool visible)
{
    m_textEdit->setLineNumberVisible(visible);
}

void MainWindow::toggleToolbar()
{
    m_fileToolBar->setVisible(m_toolbarAct->isChecked());
    m_editToolBar->setVisible(m_toolbarAct->isChecked());
    m_formatToolBar->setVisible(m_toolbarAct->isChecked());
}

void MainWindow::toggleStatusBar()
{
    statusBar()->setVisible(m_statusbarAct->isChecked());
}

void MainWindow::showAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::showFind()
{
    if (!m_findDlg->isVisible()) {
        m_findDlg->show();
    }
    m_findDlg->raise();
    m_findDlg->activateWindow();
}

void MainWindow::showReplace()
{
    if (!m_replaceDlg->isVisible()) {
        m_replaceDlg->show();
    }
    m_replaceDlg->raise();
    m_replaceDlg->activateWindow();
}

void MainWindow::documentModified(bool changed)
{
    m_isModified = changed;
    setWindowModified(changed);
}

void MainWindow::updateStatusBar()
{
    QTextCursor cursor = m_textEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    m_positionLabel->setText(tr("行: %1, 列: %2").arg(line).arg(col));

    int chars = m_textEdit->toPlainText().size();
    m_lengthLabel->setText(tr("字符数: %1").arg(chars));
}

void MainWindow::autoSave()
{
    if (m_isModified && !m_currentFile.isEmpty()) {
        saveFile(m_currentFile);
        statusBar()->showMessage(tr("自动保存成功"), 2000);
    }
}

void MainWindow::onTextChanged()
{
    documentModified(true);
    updateStatusBar();
}

bool MainWindow::maybeSave()
{
    if (!m_isModified)
        return true;

    QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("文本编辑器"),
        tr("文档已被修改。\n是否保存修改？"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        saveFile();
        return true;
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("打开"),
                             tr("无法打开文件 %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    m_textEdit->setPlainText(in.readAll());
    file.close();

    setCurrentFile(fileName);
    m_isModified = false;
    documentModified(false);
    statusBar()->showMessage(tr("文件已打开"), 2000);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_currentFile = fileName;
    m_isModified = false;
    documentModified(false);

    QString shownName = m_currentFile;
    if (shownName.isEmpty())
        shownName = "未命名.txt";

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("简单文本编辑器")));
}

void MainWindow::updateEditActions()
{
    bool hasSelection = m_textEdit->textCursor().hasSelection();
    m_cutAct->setEnabled(hasSelection);
    m_copyAct->setEnabled(hasSelection);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}
// 主窗口实现
// 状态栏更新逻辑
