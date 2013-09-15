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

#include "currentdiff.h"
#include "mainwindowrevview.h"
#include "fileschangesview.h"
#include "clonedialog.h"
#include "options.h"
#include "externaldiff.h"

namespace Ui {
class MainWindow;
}

class MainWindowRevView;
class filesChangesView;
class currentDiff;

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

    void on_actionSave_Path_triggered();

    void on_actionCreate_Tag_triggered();

    void on_actionDelete_Tag_triggered();

    void on_fullLogText_anchorClicked(const QUrl &arg1);

    void on_actionClone_triggered();

    void cloneCompleted();
    void on_actionSoft_triggered();

    void on_actionMixed_triggered();

    void on_actionHard_triggered();

    void on_actionPreferences_triggered();

    void updateExternalDiffTool(QString executableLocation);
    void optionsDialogClosed();
    void on_fileChangesView_customContextMenuRequested(const QPoint &pos);

private:

    Ui::MainWindow *ui;

    QStringList populateBranchList(QList<AcGit::Branch *> branches);
    void removeOldShownCommit();
    void setup();
    void updateTags();
    void updateBranches();
    void populateNewRepo();

    AcGit::Repository *repo;
    currentDiff *shownCommit;

    MainWindowRevView *revView;
    filesChangesView *commitChangesView;
    Options *optionsDialog;
    CloneDialog *openCloneDialog;
    QString diffExecutable;
    ExternalDiff *externalDiff;

    void buildTreeForCommit();
    bool isValidIndex(int index);
    bool openRepository(QString path);
    void addOffClickMenuItems(QMenu &menu);
    void deleteTagFromCommit(AcGit::Commit *commit);
    void addTagToCommit(AcGit::Commit *commit);
    void savePatch(AcGit::Commit *commit);
    void createBranchOnCommit(AcGit::Commit *commit);
    void deleteBranchOnCommit(AcGit::Commit *commit);
    void updatefullLogText();
    void resetAction(AcGit::IReset::resetType type);
    void checkForChanges();
    void addFileViewOffClickMenu(QMenu &menu);
};

#endif // MAINWINDOW_H

