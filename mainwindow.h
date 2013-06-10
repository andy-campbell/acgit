/**
 * This file is part of AcGit.
 *
 * AcGit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AcGit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QAbstractItemView>

#include "acrepo.h"
#include "currentdiff.h"
#include "mainwindowrevview.h"
#include "fileschangesview.h"

namespace Ui {
class MainWindow;
}

class MainWindowRevView;
class filesChangesView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void setShownCommit(int index);
    currentDiff *getShownCommit();

private slots:
    void on_actionOpen_triggered();

    void gitTreeSelectedRow(const QModelIndex &index);

    void on_fileChangesView_clicked(const QModelIndex &index);

    void on_revList_customContextMenuRequested(const QPoint &pos);

    void on_actionQuit_triggered();

    void on_branchesCombo_activated(const QString &arg1);

    void on_tagsCombo_activated(const QString &arg1);

private:

    Ui::MainWindow *ui;

    QStringList populateBranchList(QList<AcGit::Branch *> branches);
    void removeOldShownCommit();
    void setup();
    void updateTags();
    void updateBranches();
    // TODO remove from here
    //acRepo *repo;
    AcGit::Repository *repo;
    currentDiff *shownCommit;

    MainWindowRevView *revView;
    filesChangesView *commitChangesView;

    void buildTreeForCommit(const AcGit::Commit *commit);
};

#endif // MAINWINDOW_H
