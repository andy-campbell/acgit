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


    void addCommitsToView(acRepo *repo);
    void clearCommits();

    void setupDelegate(acRepo *repo);
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
    void addCommit(Commit *commit, int row);
    void addWorkingDirectoryCommit(int row);
    void setupRevViewHeaders();

    int columnSize = 550;
    int columnCount = 4;

    MainWindow *mainWindow;
    QTreeView *revView;
    QStandardItemModel *model;

};

#endif // MAINWINDOWREVVIEW_H
