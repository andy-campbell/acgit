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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../libqgit2/qgit2.h"

#include "acrepo.h"
#include "currentcommit.h"


#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QHeaderView>

#include <QHeaderView>
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QTableView>

#include "commit.h"
#include "revviewdelegate.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // null variables
    shownCommit = nullptr;
    repo = nullptr;

    // extra ui setup
    setup();

    // try open a git repo in the current directory
    try
    {
        repo = new acRepo(QDir::currentPath() + "/.git");
        revWalk();
        updateTags();
        updateBranches();
    }
    catch (LibQGit2::QGitException e)
    {
        // there is no repo in this directory or sub directories
        // which can be handled gracefully by ignoring this issue.
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    // Add header to file List
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(1);

    // setup header info for files changed and update treeview
    QStandardItem *header = new QStandardItem();
    header->setData("Files Changed", Qt::DisplayRole);
    model->setHorizontalHeaderItem(0, header);
    ui->fileChangesView->setModel(model);

    // update diff splitter.
    QList<int> sizes;
    sizes.append(ui->filesChanged->size().width() / 5);
    sizes.append(ui->filesChanged->size().width() - (ui->filesChanged->size().width() / 5));
    ui->diffSplitter->setSizes(sizes);

    // Make diff read only
    ui->diffView->setReadOnly(true);

}

/**
 * @brief MainWindow::revWalk display all of the commits in the repo
 */
void MainWindow::revWalk ()
{
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(4);

    // setup header info
    QStandardItem *header = new QStandardItem();
    header->setData("Rev", Qt::DisplayRole);
    model->setHorizontalHeaderItem(0, header);

    // setup header info
    header = new QStandardItem();
    header->setData("Short Log", Qt::DisplayRole);
    model->setHorizontalHeaderItem(1, header);

    // setup author header info
    header = new QStandardItem();
    header->setData("Author", Qt::DisplayRole);
    model->setHorizontalHeaderItem(2, header);

    //setup when author made commit header
    header = new QStandardItem();
    header->setData("Date", Qt::DisplayRole);
    model->setHorizontalHeaderItem(3, header);

    // loop over all commits and set up all useful info in the
    // rev tree view
    int row = 0;
    foreach (Commit* commit, repo->getAllCommits())
    {
        QStandardItem *index0 = new QStandardItem();
        index0->setData("", Qt::DisplayRole);
        model->setItem(row, 0, index0);

        if (commit->getCommitType() == Commit::NO_COMMIT_WORKING_DIR)
        {
            QStandardItem *index1 = new QStandardItem();
            index1->setData("Working Directory", Qt::DisplayRole);
            model->setItem(row, 1, index1);
        }
        else
        {

            QStandardItem *index1 = new QStandardItem();
            index1->setData(commit->getCommit().shortMessage(), Qt::DisplayRole);
            model->setItem(row, 1, index1);

            QStandardItem *index2 = new QStandardItem();
            index2->setData(commit->getCommit().author().name(), Qt::DisplayRole);
            model->setItem(row, 2, index2);

            QStandardItem *index3 = new QStandardItem();
            index3->setData(commit->getCommit().author().when().toString(), Qt::DisplayRole);
            model->setItem(row, 3, index3);
        }

        row++;
    }
    ui->revList->setModel(model);

    revViewDelegate *delegate = new revViewDelegate(repo, this);
    ui->revList->setItemDelegate (delegate);

    // resize short comment column so more text can be seen
    ui->revList->setColumnWidth(1, 350);
    ui->revList->show();

    // Connect item changes to callback
    connect(ui->revList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(revListSelectionChanged(QItemSelection,QItemSelection)));
}

void MainWindow::updateTags()
{
    // clear tags out if there is any
    ui->tagsCombo->clear();

    // add the new tags
    ui->tagsCombo->insertItems(0, repo->getTags());
}

void MainWindow::updateBranches()
{
    ui->branchesCombo->clear();

    ui->branchesCombo->insertItems(0, repo->getBranches());
}


void MainWindow::loadRepo()
{
    revWalk();
}

void MainWindow::on_actionOpen_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::home().path());
    try
    {
        repo = new acRepo(folderName + "/.git");
    }
    catch (LibQGit2::QGitException e)
    {
        // most likely we don't have a valid repo to open
        QMessageBox::information(this, "Cannot open repository",
                                 "There are no git repositories in this directory"
                                 "or any subdirectory.",QMessageBox::Ok);
        return;
    }


    connect (repo, SIGNAL(repoOpened()),this, SLOT(loadRepo()));
    revWalk();
    updateTags();
    updateBranches();

}

void MainWindow::gitTreeSelectedRow(const QModelIndex& index)
{
    QString path = index.data(Qt::DisplayRole).toString();
    QModelIndex parent = index.parent();

    // if it has children then it is a directory
    if (ui->gitTree->model()->hasChildren(index))
    {
        return;
    }

    while (parent.isValid())
    {
        path = parent.data().toString() + "/" + path;
        parent = parent.parent();
    }

    // get commit we are working with
    Commit *commit = repo->getAllCommits().at(ui->revList->currentIndex().row());

    // check if the commit is the dummy commit for working dir
    if (commit->getCurrentRowState()->at(0) == Commit::NO_COMMIT_WORKING_DIR)
    {

    }
    else
    {
        LibQGit2::QGitTree tree = commit->getCommit().tree().toTree();
        LibQGit2::QGitTreeEntry entry = tree.entryByName(path);
        LibQGit2::QGitObject object = entry.toObject(repo->getRepo());

        LibQGit2::QGitBlob blob = object.toBlob();

        // clear output and then append file text
        ui->fileText->clear();
        ui->fileText->append(QString((char*)blob.rawContent()));
    }

}

void MainWindow::buildTreeForCommit(const Commit *commit)
{
    // Handle Tree
    QStringList fileListTree = commit->getCommit().tree().getAllEntries();
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(1);

    QMap <int, QString> tree;
    QList<QStandardItem*> parentList;
    int row = 0;

    foreach (QString filename, fileListTree)
    {
        QStringList parsedPath = filename.split("/");
        bool newBranch = false;

        for (int i = 0; i < parsedPath.length(); i++)
        {
            newBranch = parsedPath.at(i).compare(tree[i]);
            if (newBranch != 0)
            {
                tree.insert(i, parsedPath.at(i));
                QStandardItem *index0 = new QStandardItem();
                index0->setData(parsedPath.at(i), Qt::DisplayRole);

                if (i == 0)
                {
                    model->setItem(row, 0, index0);
                    row++;

                    if (parentList.empty() == false)
                        parentList.replace(i, index0);
                    else
                        parentList.push_back(index0);
                }
                else
                {

                    parentList.at(i -1)->appendRow(index0);


                    if (i < parentList.length())
                        parentList.replace(i, index0);
                    else
                        parentList.insert(i, index0);
                }

            }
        }
    }
    connect(ui->gitTree,SIGNAL(clicked(const QModelIndex& ) ),
            this,SLOT( gitTreeSelectedRow(const QModelIndex& ) ) );

    connect(ui->gitTree,SIGNAL(customContextMenuRequested(QPoint)) ,
            this,SLOT( on_revList_customContextMenuRequested(QPoint) ) );
    ui->gitTree->setModel(model);
    ui->gitTree->header()->close();

}

void MainWindow::revListSelectionChanged(QItemSelection selected,QItemSelection deSelected)
{
    // Handle file list
    if (shownCommit)
    {
        delete shownCommit;
    }

    QModelIndex index = ui->revList->currentIndex();

    shownCommit = new currentCommit (repo, repo->getAllCommits().at(index.row() + 1), repo->getAllCommits().at(index.row()));

    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(1);

    // setup header info for files changed and update treeview
    QStandardItem *header = new QStandardItem();
    header->setData("Files Changed", Qt::DisplayRole);
    model->setHorizontalHeaderItem(0, header);

    int row = 0;
    foreach (QString file, shownCommit->getFileList())
    {
        //create items and then add it to the model
        QStandardItem *index = new QStandardItem();
        index->setData(file, Qt::DisplayRole);
        model->setItem(row, 0, index);

        row++;
    }

    // set diffview to show the delta of the first file
    ui->diffView->clear();
    ui->diffView->append(shownCommit->getDetaForFile(shownCommit->getFileList().first()));

    // TODO sort this out this is duplicate code and is bad practice
    ui->fileChangesView->setModel(model);
    QModelIndex firstItem = model->index(0,0);
    ui->fileChangesView->setCurrentIndex(firstItem);

    // Handle Long message
    ui->fullLogText->clear();
    if (shownCommit->getCurrentSelectedCommit()->getCommitType() == Commit::NO_COMMIT_WORKING_DIR)
    {
        ui->fullLogText->append ("Changes in working directory");
    }
    else
    {
        ui->fullLogText->append(shownCommit->getCurrentSelectedCommit()->getCommit().oid().format() + "\n");
        ui->fullLogText->append(shownCommit->getCurrentSelectedCommit()->getCommit().message());
        buildTreeForCommit(shownCommit->getCurrentSelectedCommit());
    }


}

void MainWindow::on_fileChangesView_clicked(const QModelIndex &index)
{
    QString file = index.data(Qt::DisplayRole).toString();

    ui->diffView->clear();
    ui->diffView->append(shownCommit->getDetaForFile(file));
}

void MainWindow::on_revList_customContextMenuRequested(const QPoint &pos)
{
   QModelIndex index = ui->revList->indexAt(pos);
   QPoint globalPos = ui->revList->mapToGlobal(pos);

   qDebug() << index.row();
   QMenu menu(this);
   menu.addAction("Save Patch");

   QAction* selectedItem = menu.exec(globalPos);
   if (selectedItem)
   {
       Commit *commitToSave = repo->getAllCommits().at(index.row());
       QString exampleName = QDir::currentPath() + "/" + commitToSave->getCommit().shortMessage().replace(" ", "_")
               + ".patch";
       QString patchName = QFileDialog::getSaveFileName (this, tr("Save Patch"),
                                                         exampleName ,tr("Patches (*.patch)") );

       qDebug () << patchName;
       shownCommit->savePatch(patchName);
   }
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::exit();
}

void MainWindow::on_branchesCombo_activated(const QString &arg1)
{
    int lookupIndex = repo->lookupBranch(arg1);

    // if value is -1 then an error has occured in lookup so don't change index
    if (lookupIndex != -1)
    {
        QModelIndex modIndex = ui->revList->model()->index(lookupIndex, 0);
        ui->revList->setCurrentIndex(modIndex);
    }

}

void MainWindow::on_tagsCombo_activated(const QString &arg1)
{
    int lookupIndex = repo->lookupTag(arg1);

    // if value is -1 then an error has occured in lookup so don't change index
    if (lookupIndex != -1)
    {
        QModelIndex modIndex = ui->revList->model()->index(lookupIndex, 0);
        ui->revList->setCurrentIndex(modIndex);
    }
}
