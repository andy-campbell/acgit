#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QAbstractItemView>

#include "acrepo.h"
#include "currentcommit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void revWalk();
    void findAllBranches();
private slots:
    void on_actionOpen_triggered();
    void loadRepo();
    void gitTreeSelectedRow(const QModelIndex &index);

    void on_revList_clicked(const QModelIndex &index);

    void on_fileChangesView_clicked(const QModelIndex &index);

private:

    Ui::MainWindow *ui;

    void setup();
    // TODO remove from here
    acRepo *repo;
    currentCommit *shownCommit;

    void buildTreeForCommit(const Commit *commit);
};

#endif // MAINWINDOW_H
