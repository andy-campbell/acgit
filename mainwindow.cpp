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

#include <QInputDialog>

#include "commit.h"
#include "revviewdelegate.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    revView = new MainWindowRevView(this, ui->revList);
    commitChangesView = new filesChangesView(this, ui->diffView, ui->fileChangesView);
    // null variables
    repo = nullptr;
    shownCommit = nullptr;

    // extra ui setup
    setup();

    // try open a git repo in the current directory
    try
    {
        repo = new acRepo(QDir::currentPath() + "/.git");
        revView->addCommitsToView(repo);
        revView->setupDelegate(repo);
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

void MainWindow::removeOldShownCommit()
{
    if (shownCommit)
    {
        delete shownCommit;
    }
}

void MainWindow::setShownCommit(int index)
{
    removeOldShownCommit();
    shownCommit = new currentCommit (repo, repo->getAllCommits().at(index + 1), repo->getAllCommits().at(index));

    commitChangesView->update(shownCommit);

    // Handle Long message
    ui->fullLogText->clear();
    if (shownCommit->getCurrentSelectedCommit()->getCommitType() == Commit::NO_COMMIT_WORKING_DIR)
    {
        ui->fullLogText->append ("Changes in working directory");
    }
    else
    {
        ui->fullLogText->append(shownCommit->getCurrentSelectedCommit()->getCommit().message());
        ui->fullLogText->append("\n\n\n");
        ui->fullLogText->append(shownCommit->getPatchStats());
        buildTreeForCommit(shownCommit->getCurrentSelectedCommit());
    }

}

currentCommit *MainWindow::getShownCommit()
{
    return shownCommit;
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

void MainWindow::on_actionOpen_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::home().path());
    try
    {
        if (repo)
        {
            // already have a repo so do some tidy up
            revView->setupDelegate(nullptr);
            delete repo;
        }

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


    revView->addCommitsToView(repo);
    revView->setupDelegate(repo);
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
    if (commit->getCurrentRowState()->at(0) != Commit::NO_COMMIT_WORKING_DIR)
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

                    parentList.at(i - 1)->appendRow(index0);


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

    QMenu menu(this);
    menu.addAction(tr("Save Patch"));
    menu.addAction(tr("Add Tag"));
    menu.addAction(tr("Delete Tag"));
    menu.addSeparator();
    menu.addAction(tr("Create Branch"));
    menu.addAction(tr("Delete Branch"));
    menu.addAction(tr("Checkout branch"));


    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        Commit *commit = repo->getAllCommits().at(index.row());
        if (selectedItem->iconText().contains(tr("Add Tag")))
        {
            // Prompt user for tag name
            QString tagName = QInputDialog::getText (this, tr("Please enter tag name"), tr("Tag name"));

            // Add Tag
            if (!tagName.isEmpty())
                commit->createTag(repo, tagName);
        }
        else if (selectedItem->iconText().contains(tr("Remove Tag")))
        {
            QStringList tagsList = commit->getTags();

            if (tagsList.size() == 1)
            {
                commit->removeTag(repo, tagsList.first());
            }
            else if (tagsList.size() > 1)
            {
                QString tag = QInputDialog::getItem(this, tr("Remove tag"), tr("Please select tag to remove"), tagsList);
                commit->removeTag(repo, tag);
            }

        }
        else if (selectedItem->iconText().contains(tr("Save Patch")))
        {
            QString exampleName = QDir::currentPath() + "/" + commit->getCommit().shortMessage().replace(" ", "_")
                       + ".patch";
               QString patchName = QFileDialog::getSaveFileName (this, tr("Save Patch"),
                                                                 exampleName ,tr("Patches (*.patch)") );

            shownCommit->savePatch(patchName);
        }
        else if (selectedItem->iconText().contains(tr("Create Branch")))
        {
            // Prompt user for tag name
            QString branchName = QInputDialog::getText (this, tr("Please enter branch name"), tr("Branch name"));

            // Add Branch
            if (!branchName.isNull())
                commit->createBranch(repo, branchName);
        }
        else if (selectedItem->iconText().contains(tr("Delete Branch")))
        {
            QStringList branchList = commit->getBranches();


            if (branchList.size() == 1)
            {
                int proceed = QMessageBox::question(this, tr("Are you sure"), tr("Are you sure you wish to delete %1").arg(branchList.first()),
                                                     QMessageBox::Yes, QMessageBox::No);

                if (proceed)
                    commit->removeBranch(repo, branchList.first());
            }
            else if (branchList.size() > 1)
            {
                QString branch = QInputDialog::getItem(this, tr("Remove tag"), tr("Please select tag to remove"), branchList);

                int proceed = QMessageBox::question(this, tr("Are you sure"), tr("Are you sure you wish to delete %1").arg(branchList.first()),
                                                     QMessageBox::Yes, QMessageBox::No);
                if (proceed)
                    commit->removeBranch(repo, "refs/heads/" + branch);
            }
        }
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
