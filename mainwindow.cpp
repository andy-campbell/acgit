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

        QStandardItem *index1 = new QStandardItem();
        index1->setData(commit->getCommit().shortMessage(), Qt::DisplayRole);
        model->setItem(row, 1, index1);

        QStandardItem *index2 = new QStandardItem();
        index2->setData(commit->getCommit().author().name(), Qt::DisplayRole);
        model->setItem(row, 2, index2);

        QStandardItem *index3 = new QStandardItem();
        index3->setData(commit->getCommit().author().when().toString(), Qt::DisplayRole);
        model->setItem(row, 3, index3);

        row++;
    }
    ui->revList->setModel(model);

    revViewDelegate *delegate = new revViewDelegate(repo, this);
    ui->revList->setItemDelegate (delegate);

    // resize short comment column so more text can be seen
    ui->revList->setColumnWidth(1, 350);
    ui->revList->show();
}

void MainWindow::loadRepo()
{
    revWalk();
}

void MainWindow::findAllBranches()
{

}


void MainWindow::on_actionOpen_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::home().path());
    repo = new acRepo(folderName + "/.git");

    connect (repo, SIGNAL(repoOpened()),this, SLOT(loadRepo()));
    revWalk();
    findAllBranches();
}

void MainWindow::gitTreeSelectedRow(const QModelIndex& index)
{
    QString path = index.data(Qt::DisplayRole).toString();
    QModelIndex parent = index.parent();

    while (parent.isValid())
    {
        path = parent.data().toString() + "/" + path;
        parent = parent.parent();
    }

    // get comming we are working with
    Commit *commit = repo->getAllCommits().at(ui->revList->currentIndex().row());

    LibQGit2::QGitTree tree = commit->getCommit().tree().toTree();
    LibQGit2::QGitTreeEntry entry = tree.entryByName(path);
    LibQGit2::QGitObject object = entry.toObject(repo->getRepo());

    LibQGit2::QGitBlob blob = object.toBlob();

    // clear output and then appent file text
    ui->fileText->clear();
    ui->fileText->append(QString((char*)blob.rawContent()));

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

    ui->gitTree->setModel(model);
    ui->gitTree->header()->close();

}

void MainWindow::on_revList_clicked(const QModelIndex &index)
{
    // Handle file list
    if (shownCommit)
    {
        delete shownCommit;
    }

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
    ui->fullLogText->append(shownCommit->getCurrentSelectedCommit()->getCommit().message());

    buildTreeForCommit(shownCommit->getCurrentSelectedCommit());

}

void MainWindow::on_fileChangesView_clicked(const QModelIndex &index)
{
    QString file = index.data(Qt::DisplayRole).toString();

    ui->diffView->clear();
    ui->diffView->append(shownCommit->getDetaForFile(file));
}
