#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLabel>
#include <QTimer>
#include "codeeditor.h"

class FindDialog;
class ReplaceDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件操作
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();

    // 编辑操作
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();

    // 格式操作
    void setFont();
    void setFontColor();
    void setBackgroundColor();
    void toggleWrap();
    void toggleLineNumber(bool visible);

    // 视图
    void toggleToolbar();
    void toggleStatusBar();

    // 对话框
    void showAbout();
    void showFind();
    void showReplace();

    // 状态更新
    void documentModified(bool changed);
    void updateStatusBar();
    void autoSave();
    void onTextChanged();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWidgets();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateEditActions();

    CodeEditor *m_textEdit;
    FindDialog *m_findDlg;
    ReplaceDialog *m_replaceDlg;

    QLabel *m_positionLabel;
    QLabel *m_lengthLabel;

    // 菜单
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_formatMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;

    // 工具栏
    QToolBar *m_fileToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_formatToolBar;

    // Action - 文件
    QAction *m_newAct;
    QAction *m_openAct;
    QAction *m_saveAct;
    QAction *m_saveAsAct;
    QAction *m_exitAct;

    // Action - 编辑
    QAction *m_undoAct;
    QAction *m_redoAct;
    QAction *m_cutAct;
    QAction *m_copyAct;
    QAction *m_pasteAct;
    QAction *m_selectAllAct;
    QAction *m_findAct;
    QAction *m_replaceAct;

    // Action - 格式
    QAction *m_fontAct;
    QAction *m_fontColorAct;
    QAction *m_bgColorAct;
    QAction *m_wrapAct;
    QAction *m_lineNumberAct;

    // Action - 视图
    QAction *m_toolbarAct;
    QAction *m_statusbarAct;

    // Action - 帮助
    QAction *m_aboutAct;

    QString m_currentFile;
    bool m_isModified;
    QTimer *m_autoSaveTimer;
};

#endif // MAINWINDOW_H
