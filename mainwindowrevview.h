#ifndef MAINWINDOWREVVIEW_H
#define MAINWINDOWREVVIEW_H

#include <QObject>
#include <QStandardItemModel>
#include <QTreeView>

#include "mainwindow.h"

class MainWindow;

class MainWindowRevView : public QObject
{
    Q_OBJECT
public:
    MainWindowRevView(MainWindow *_mainWindow, QTreeView *_revView);
    virtual ~MainWindowRevView();


    void addCommitsToView(AcGit::Repository *repo);
    void clearCommits();

    void setupDelegate(AcGit::Repository *repo);
    void checkForWorkingDirectoryChanges(AcGit::Repository *repo);
    void checkForStagingDirectoryChanges(AcGit::Repository *repo);
    const bool hasWorkingDirectoryChanges() const;
    const bool hasStagingDirectoryChanges() const;

private slots:
    void revViewSelectionChanged(QItemSelection selected,QItemSelection deSelected);
private:

    enum
    {
        REV_INDEX = 0,
        SHORT_LOG_INDEX,
        AUTHOR_INDEX,
        DATE_INDEX
    } rowIndex;

    void addColumnData(int row, int column, QString columnContents);
    void setHeaderName(QString name, int index);
    void setupSelectionChangedCallBack();
    void setRevViewModel();
    void setColumnWidth();
    void addCommit(AcGit::Commit *commit, int row);
    void addWorkingDirectoryCommit();
    void addStagingDirectoryCommit();
    void setupRevViewHeaders();

    int columnSize = 550;
    int columnCount = 4;
    const int working_dir_row = 0;
    MainWindow *mainWindow;
    QTreeView *revView;
    QStandardItemModel *model;
    bool hasWorkingChanges;
    bool hasStagingChanges;

};

#endif // MAINWINDOWREVVIEW_H
