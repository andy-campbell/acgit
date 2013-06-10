#ifndef FILESCHANGESVIEW_H
#define FILESCHANGESVIEW_H

#include <QObject>
#include <QStandardItemModel>

#include "../libAcGit/AcGitGlobal.h"
#include "mainwindow.h"
#include "difftextedit.h"

class MainWindow;

class filesChangesView : public QObject
{
    Q_OBJECT
public:
    filesChangesView(MainWindow *_mainWindow, diffTextEdit *_selectedCommitDiff, QTreeView *_fileChanged);
    
    void clearOldChangedFilesList();
    void update(currentDiff *shownCommit);
private:
    void setupHeader();
    void setupModel();
    void updateFileDiff(currentDiff *shownCommit, QString fileName);
    void updateChangedFiles(QStringList changeFileList);
    void addNewChangedFileList(QStringList changedFileList);

    QStandardItemModel *model;
    MainWindow *mainWindow;
    QTreeView *filesChanged;
    diffTextEdit *selectedCommitDiff;
};

#endif // FILESCHANGESVIEW_H
