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
#include <QList>
#include <QInputDialog>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "mainwindow.h"

#include "currentdiff.h"
#include "revviewdelegate.h"
#include "AcGitGlobal.h"

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
    openCloneDialog = nullptr;

    // extra ui setup
    setup();

    // try open a git repo in the current directory
    if (openRepository(QDir::currentPath()))
    {
        populateNewRepo();
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

bool MainWindow::isValidIndex(int index)
{
    AcGit::ICommits *commitsAgent = repo->CommitsAgent();

    if(index < 0)
    {
        return false;
    }

    int commitIndex = revView->findCommitIndex(index);
    if(commitIndex >= commitsAgent->getAllCommits()->length())
    {
        return false;
    }

    return true;
}

void MainWindow::updatefullLogText()
{
    // Handle Long message
    ui->fullLogText->clear();

    ui->fullLogText->insertHtml("<!DOCTYPE html>");
    ui->fullLogText->insertHtml("<html>");
    if (shownCommit->getType() == WORKINGDIR)
    {
        if (shownCommit->getFileList().size() == 0)
        {
            ui->fullLogText->append ("No current in working directory");
        }
        else
        {
            ui->fullLogText->append ("Changes in working directory");
        }
    }
    else if (shownCommit->getType() == WORKINGDIR)
    {
        if (shownCommit->getFileList().size() == 0)
        {
            ui->fullLogText->append ("No current in staging directory");
        }
        else
        {
            ui->fullLogText->append ("Changes in staging directory");
        }

    }
    else if (shownCommit->getType() == COMMIT)
    {
        AcGit::Commit *commit = shownCommit->getCurrentSelectedCommit();
        ui->fullLogText->insertHtml(QString("<h3>%1</h3><br>").arg(commit->shortLog()));

        for (int parentIndex = 0; parentIndex < commit->parentCount(); parentIndex++)
        {
            AcGit::Sha *parentSha = commit->parentSha(parentIndex);
            QString shaString = parentSha->toString();
            ui->fullLogText->insertHtml(QString("<p><b>Parent:</b> <a href=%1>%2</a></p><br>").arg(shaString).arg(shaString));
        }

        ui->fullLogText->insertHtml("<hr><br />");
        const QString message = shownCommit->getCurrentSelectedCommit()->log();
        ui->fullLogText->insertHtml(QString("<p>%1</p>").arg(message));

    }
    ui->fullLogText->insertHtml("</html>");
}

void MainWindow::setShownCommit(int index)
{
    if(isValidIndex(index) == false)
    {
        return;
    }

    removeOldShownCommit();

    int commitIndex = revView->findCommitIndex(index);

    if(commitIndex < 0)
    {
        if(revView->hasWorkingDirectoryChanges() && index == 0)
        {
            shownCommit = new currentDiff(repo->HeadCommit(), true);
        }

        if(revView->hasStagingDirectoryChanges() && index == 1)
        {
            shownCommit = new currentDiff(repo->HeadCommit(), false);
        }

        if(revView->hasStagingDirectoryChanges() && !revView->hasWorkingDirectoryChanges() && index == 0)
        {
            shownCommit = new currentDiff(repo->HeadCommit(), false);
        }
    }
    else
    {
        AcGit::ICommits *commitsAgent = repo->CommitsAgent();
        AcGit::Commit *to = commitsAgent->getAllCommits()->at(commitIndex);
        AcGit::Commit *from;
        int nextIndex = index + 1;

        if (nextIndex == commitsAgent->getAllCommits()->count())
        {
            from = nullptr;;
        }
        else
        {
            from = commitsAgent->getAllCommits()->at(nextIndex);
        }
        shownCommit = new currentDiff (from, to);
    }

    commitChangesView->update(shownCommit);

    updatefullLogText();
}

currentDiff *MainWindow::getShownCommit()
{
    return shownCommit;
}

void MainWindow::updateTags()
{
    // clear tags out if there is any
    ui->tagsCombo->clear();

    // add the new tags
    AcGit::ITags *tagsAgent = repo->TagsAgent();
    ui->tagsCombo->insertItems(0, tagsAgent->stringTagsList());
}

void MainWindow::updateBranches()
{
    ui->branchesCombo->clear();

    AcGit::IBranches *branchAgent = repo->BranchAgent();
    ui->branchesCombo->insertItems(0, branchAgent->branchNamesList());

    QString currentBranch = branchAgent->currentBranch();
    // -1 to remove the '/'
    currentBranch = currentBranch.right(currentBranch.length() - currentBranch.lastIndexOf('/') - 1);
    ui->branchLabel->setText(currentBranch);

}

bool MainWindow::openRepository(QString path)
{
    bool openedSuccessfully = false;
    try
    {
        if (repo)
        {
            // already have a repo so do some tidy up
            revView->setupDelegate(nullptr);
            delete repo;
            repo = nullptr;
        }

        repo = new AcGit::Repository(path);
        openedSuccessfully = true;
    }
    catch (AcGit::GitException e)
    {
        // most likely we don't have a valid repo to open
        QMessageBox::information(this, "Cannot open repository",
                                 "There are no git repositories in this directory"
                                 "or any subdirectory.",QMessageBox::Ok);
    }
    return openedSuccessfully;

}

void MainWindow::populateNewRepo()
{
    revView->checkForWorkingDirectoryChanges(repo);
    revView->checkForStagingDirectoryChanges(repo);
    revView->addCommitsToView(repo);
    revView->setupDelegate(repo);
    updateTags();
    updateBranches();
}

void MainWindow::on_actionOpen_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::home().path());

    if(folderName.length() == 0)
    {
        return;
    }

    if (openRepository(folderName))
    {
        populateNewRepo();
    }

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

     //get commit we are working with
    int row = ui->revList->currentIndex().row();

    if (repo->HasWorkingTreeChanges())
    {
        // to allow for commit array to start from 0
        row -= 1;
    }

    AcGit::ICommits *commitsAgent = repo->CommitsAgent();
    AcGit::Commit *commit = commitsAgent->getAllCommits()->at(row);

    AcGit::Tree *tree = commit->tree();

    AcGit::TreeEntry *entry = tree->getEntry(path);
    if (entry == nullptr)
    {
        qDebug() << "entry is null";
        return;
    }
    AcGit::Blob blob = entry->fileBlob();

    // clear output and then append file text
    ui->fileText->clear();
    ui->fileText->append(blob.contents());

}

void MainWindow::buildTreeForCommit(AcGit::Commit *commit)
{
    // Handle Tree
    QList<AcGit::TreeEntry*> entries = commit->tree()->getEntriesList();
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(1);

    QMap <int, QString> tree;
    QList<QStandardItem*> parentList;
    int row = 0;

    foreach (AcGit::TreeEntry *treeEntry, entries)
    {
        QString path = treeEntry->fullPath();
        QStringList parsedPath = path.split("/");
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

QStringList MainWindow::populateBranchList(QList<AcGit::Branch *> branches)
{
    QStringList branchStringList;

    foreach (AcGit::Branch *branch, branches)
    {
        branchStringList.append(branch->getBranchName());
    }
    return branchStringList;
}

void MainWindow::addOffClickMenuItems(QMenu &menu)
{
    menu.addAction(tr("Save Patch"));
    menu.addAction(tr("Add Tag"));
    menu.addAction(tr("Delete Tag"));
    menu.addSeparator();
    menu.addAction(tr("Create Branch"));
    menu.addAction(tr("Delete Branch"));
    menu.addAction(tr("Checkout Branch"));
    menu.addSeparator();
    menu.addAction(tr("Reset Soft"));
    menu.addAction(tr("Reset Mixed"));
    menu.addAction(tr("Reset Hard"));

}

void MainWindow::deleteTagFromCommit(AcGit::Commit *commit)
{
    AcGit::ITags *tagsAgent = repo->TagsAgent();
    QList<AcGit::Tag *> tags = tagsAgent->lookupTag(commit);

    QStringList tagNames;
    foreach (AcGit::Tag *tag, tags)
    {
        tagNames << tag->name();
    }

    if (tagNames.size() == 1)
    {
        tagsAgent->deleteTag(tagsAgent->lookupTag(tagNames.first()));
    }
    else if (tagNames.size() >= 1)
    {
        QString tag = QInputDialog::getItem(this, tr("Remove tag"), tr("Please select tag to remove"), tagNames);
        tagsAgent->deleteTag(tagsAgent->lookupTag(tag));
    }

}


void MainWindow::addTagToCommit(AcGit::Commit *commit)
{
    // Prompt user for tag name
    QString tagName = QInputDialog::getText (this, tr("Please enter tag name"), tr("Tag name"));

    // Add Tag
    if (!tagName.isEmpty())
    {
        AcGit::Configuration *config = repo->ConfigurationAgent();
        QString name = config->userName();
        QString email = config->userEmail();
        AcGit::Tagger *tagger = new AcGit::Tagger(name, email);
        QString message;

        int ret = QMessageBox::question(this, tr("Tag message"), tr("Would you like to add a message?"),
                               QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape);

        if(ret == QMessageBox::Yes)
        {
            message = QInputDialog::getText (this, tr("Please enter messsage"), tr("Tag message"));
        }


        repo->TagsAgent()->createTag(tagName, commit, tagger, message);
    }
}

void MainWindow::savePatch(AcGit::Commit *commit)
{
    QString exampleName = QDir::currentPath() + "/" + commit->shortLog().replace(" ", "_")
               + ".patch";
    QString patchName = QFileDialog::getSaveFileName (this, tr("Save Patch"),
                                                         exampleName ,tr("Patches (*.patch)") );

    shownCommit->savePatch(patchName);
}

void MainWindow::createBranchOnCommit(AcGit::Commit *commit)
{
    QString branchName = QInputDialog::getText (this, tr("Please enter branch name"), tr("Branch name"));

    if (!branchName.isNull())
    {
        AcGit::IBranches *branchAgent = repo->BranchAgent();
        branchAgent->createNewBranch(branchName, commit);
    }
}

void MainWindow::deleteBranchOnCommit(AcGit::Commit *commit)
{
    AcGit::IBranches *branchAgent = repo->BranchAgent();
    QList<AcGit::Branch *> branchList = branchAgent->lookupBranch(commit);

    if (branchList.size() == 1)
    {
        AcGit::Branch *branch = branchList.at(0);
        int proceed = QMessageBox::question(this, tr("Are you sure"), tr("Are you sure you wish to delete %1").arg(branch->getBranchName()),
                                             QMessageBox::Yes, QMessageBox::No);

        if (proceed)
        {
            branchAgent->deleteBranch(branch);
        }
    }
    else if (branchList.size() > 1)
    {
        QStringList branchStringList;
        branchStringList = populateBranchList(branchList);
        QString branchName = QInputDialog::getItem(this, tr("Remove branch"), tr("Please select branch to remove"), branchStringList);

        int proceed = QMessageBox::question(this, tr("Are you sure"), tr("Are you sure you wish to delete %1").arg(branchName),
                                             QMessageBox::Yes, QMessageBox::No);

        if (proceed)
        {
            AcGit::Branch *branch = branchAgent->lookupBranch(branchName);
            branchAgent->deleteBranch(branch);
        }
    }
}

void MainWindow::on_revList_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->revList->indexAt(pos);
    QPoint globalPos = ui->revList->mapToGlobal(pos);

    QMenu menu(this);
    addOffClickMenuItems(menu);

    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        int commitIndex = revView->findCommitIndex(index.row());

        AcGit::ICommits *commitsAgent = repo->CommitsAgent();
        AcGit::Commit *commit = commitsAgent->getAllCommits()->at(commitIndex);
        if(selectedItem->iconText().contains(tr("Add Tag")))
        {
            addTagToCommit(commit);
        }
        else if(selectedItem->iconText().contains(tr("Delete Tag")))
        {
            deleteTagFromCommit(commit);
        }
        else if(selectedItem->iconText().contains(tr("Save Patch")))
        {
            savePatch(commit);
        }
        else if(selectedItem->iconText().contains(tr("Create Branch")))
        {
            createBranchOnCommit(commit);
        }
        else if(selectedItem->iconText().contains(tr("Delete Branch")))
        {
            deleteBranchOnCommit(commit);
        }
        else if(selectedItem->iconText().contains(tr("Reset Soft")))
        {
            resetAction(AcGit::IReset::SOFT);
        }
        else if(selectedItem->iconText().contains(tr("Reset Mixed")))
        {
            resetAction(AcGit::IReset::MIXED);
        }
        else if(selectedItem->iconText().contains(tr("Reset Hard")))
        {
            resetAction(AcGit::IReset::HARD);
        }
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::exit();
}

void MainWindow::on_branchesCombo_activated(const QString &arg1)
{
    int index = -1;
    AcGit::IBranches *branchesAgent = repo->BranchAgent();
    AcGit::ICommits *commitAgent = repo->CommitsAgent();

    QString branchName = "refs/heads/" + arg1;
    AcGit::Branch *branch = branchesAgent->lookupBranch(branchName);
    if (branch == nullptr)
    {
        // no branch found
        return;
    }

    AcGit::Commit *commit = commitAgent->lookupCommit(branch);
    if (commit == nullptr)
    {
        // no commit found
        return;
    }

    index = commitAgent->getAllCommits()->indexOf(commit);
    if (index != -1)
    {
        if (repo->HasWorkingTreeChanges())
        {
            // Add 1 for the working directory
            index += 1;
        }

        if (revView->hasWorkingDirectoryChanges())
        {
            index++;
        }

        QModelIndex modIndex = ui->revList->model()->index(index + 1 , 0);
        ui->revList->setCurrentIndex(modIndex);
    }

}

void MainWindow::on_tagsCombo_activated(const QString &arg1)
{
    int commitIndex = -1;
    AcGit::ITags *tagAgent = repo->TagsAgent();
    AcGit::ICommits *commitAgent = repo->CommitsAgent();

    AcGit::Tag *tag = tagAgent->lookupTag(arg1);
    if (tag == nullptr)
    {
        // no tag found
        return;
    }

    AcGit::Commit *commit = commitAgent->lookupCommit(tag->tagTarget());
    if (commit == nullptr)
    {
        // no commit found
        return;
    }

    commitIndex = commitAgent->getAllCommits()->indexOf(commit);
    if (commitIndex != -1)
    {
        if (revView->hasStagingDirectoryChanges())
        {
            commitIndex ++;
        }

        if (revView->hasWorkingDirectoryChanges())
        {
            commitIndex++;
        }


        QModelIndex modIndex = ui->revList->model()->index(commitIndex , 0);
        ui->revList->setCurrentIndex(modIndex);
    }
}

void MainWindow::on_actionSave_Path_triggered()
{
    int commitIndex = revView->findCommitIndex(ui->revList->currentIndex().row());

    AcGit::ICommits *commitsAgent = repo->CommitsAgent();
    AcGit::Commit *commit = commitsAgent->getAllCommits()->at(commitIndex);

    savePatch(commit);
}

void MainWindow::on_actionCreate_Tag_triggered()
{
    int commitIndex = revView->findCommitIndex(ui->revList->currentIndex().row());

    AcGit::ICommits *commitsAgent = repo->CommitsAgent();
    AcGit::Commit *commit = commitsAgent->getAllCommits()->at(commitIndex);

    addTagToCommit(commit);
}

void MainWindow::on_actionDelete_Tag_triggered()
{
    int commitIndex = revView->findCommitIndex(ui->revList->currentIndex().row());

    AcGit::ICommits *commitsAgent = repo->CommitsAgent();
    AcGit::Commit *commit = commitsAgent->getAllCommits()->at(commitIndex);

    deleteTagFromCommit(commit);
}



void MainWindow::on_fullLogText_anchorClicked(const QUrl &arg1)
{
    AcGit::Sha *sha = new AcGit::Sha(arg1.toString());

    AcGit::ICommits *commitAgent = repo->CommitsAgent();
    AcGit::Commit *commit = commitAgent->lookupCommit(sha);

    int index = commitAgent->getAllCommits()->indexOf(commit);
    revView->setActiveCommit(index);

}

void MainWindow::on_actionClone_triggered()
{
    if (repo)
    {
        // already have a repo so do some tidy up
        revView->setupDelegate(nullptr);
        delete repo;
    }

    openCloneDialog = new CloneDialog(this);
    connect(openCloneDialog,SIGNAL(cloneCompleted()),
            this,SLOT(cloneCompleted()) );

    openCloneDialog->show();
}

void MainWindow::cloneCompleted()
{
    repo = openCloneDialog->getClonedRepo();
    delete openCloneDialog;
    openCloneDialog = nullptr;
    populateNewRepo();
}

void MainWindow::checkForChanges()
{
    revView->checkForStagingDirectoryChanges(repo);
    revView->checkForWorkingDirectoryChanges(repo);
}

void MainWindow::resetAction(AcGit::IReset::resetType type)
{
    AcGit::IReset *resetAgent = repo->ResetAgent();
    AcGit::Commit *targetToResetTo = shownCommit->getCurrentSelectedCommit();

    resetAgent->resetToTarget(targetToResetTo, type);

    populateNewRepo();

}

void MainWindow::on_actionSoft_triggered()
{
    resetAction(AcGit::IReset::SOFT);
}

void MainWindow::on_actionMixed_triggered()
{
    resetAction(AcGit::IReset::MIXED);
}

void MainWindow::on_actionHard_triggered()
{
    resetAction(AcGit::IReset::HARD);
}
