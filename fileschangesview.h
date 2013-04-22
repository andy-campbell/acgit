#ifndef FILESCHANGESVIEW_H
#define FILESCHANGESVIEW_H

#include <QObject>

#include "mainwindow.h"
#include "difftextedit.h"
#include <QStandardItemModel>

class MainWindow;

class filesChangesView : public QObject
{
    Q_OBJECT
public:
    filesChangesView(MainWindow *_mainWindow, diffTextEdit *_selectedCommitDiff, QTreeView *_fileChanged);
    
    void clearOldChangedFilesList();
    void update(currentCommit *shownCommit);
private:
    void setupHeader();
    void setupModel();
    void updateFileDiff(currentCommit *shownCommit, QString fileName);
    void updateChangedFiles(QStringList changeFileList);
    void addNewChangedFileList(QStringList changedFileList);

    QStandardItemModel *model;
    MainWindow *mainWindow;
    QTreeView *filesChanged;
    diffTextEdit *selectedCommitDiff;
};

#endif // FILESCHANGESVIEW_H
