#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../libqgit2/qgit2.h"

#include "acrepo.h"


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

    //set column count to 4
    // TODO make change back to 4
    //ui->revList->setColumnCount(5);

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::revWalk walk the full repo
 * TODO make this less static
 */
void MainWindow::revWalk ()
{
//    if (repo == NULL)
//    {
//        QMessageBox msgBox;
//        msgBox.setText("No git repository selected");
//        msgBox.setInformativeText("Please open one!");
//        msgBox.exec();

//        return;
//    }

//    while (walker.next(walkCommit) == true)
//    {
//        QTreeWidgetItem *commit = new QTreeWidgetItem(ui->revList);

//        // set short message
//        commit->setText(1, walkCommit.shortMessage());

//        // set author
//        commit->setText(2, walkCommit.author().name());

//        //set author date
//        commit->setText(3, walkCommit.author().when().toString());

//        //TODO remove me - sets branch name
////        LibQGit2::QGitRef *ref = new LibQGit2::QGitRef ();

////        try
////        {
////            ref->setOId(walkCommit.oid());
////        }
////        catch (...)
////        {
////            qDebug() << "An expection occured";
////        }

////        commit->setText(4, ref->name());
//    }

}

void MainWindow::loadRepo()
{
    revWalk();
}

void MainWindow::findAllBranches()
{

    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(5);

    qDebug() << "got to here";
    int row = 0;
    foreach (Commit* commit, repo->getAllCommits())
    {
        qDebug() << row;
        QStandardItem *index0 = new QStandardItem();
        index0->setData("", Qt::DisplayRole);
        model->setItem(row, 0, index0);

        QStandardItem *index1 = new QStandardItem();
        index1->setData(commit->getCommit().shortMessage(), Qt::DisplayRole);
        model->setItem(row, 1, index1);

//        QStandardItem *index2 = new QStandardItem(commit->getCommit().author().name());
//        model.setItem(row, 2, index2);

//        QStandardItem *index3 = new QStandardItem(commit->getCommit().author().when().toString());
//        model.setItem(row, 3, index3);

//        QStandardItem *index0 = new QStandardItem();
//        model.setItem(row, 0, index0);
//        QModelIndex index1 = model.index(row, 1, QModelIndex());
//        model.setData(index1, QVariant(commit->getCommit().shortMessage()));
//        QModelIndex index2 = model.index(row, 2, QModelIndex());
//        model.setData(index2, QVariant(commit->getCommit().author().name()));
//        QModelIndex index3 = model.index(row, 3, QModelIndex());
//        model.setData(index3, QVariant(commit->getCommit().author().when().toString()));

//        model.insertRow(row, model);
        row++;
    }
    ui->revList->setModel(model);

    revViewDelegate *delegate = new revViewDelegate(repo, this);
    ui->revList->setItemDelegate (delegate);
    ui->revList->show();

//    tableView.setWindowTitle(QObject::tr("Spin Box Delegate"));
//    tableView.show();

//    QVector<Commit*> commits = repo->getAllCommits();
//    QList<QTreeWidgetItem*> rowList;
//    QTreeWidgetItem *treeCommit;

//    foreach (Commit* commit, commits)
//    {
//        //qDebug() << QString::number(commit->getCommitType()) << " " << commit->getRow() << " " << commit->getCommit().shortMessage();

//        if (commit->getRow() == 0)
//        {
//            if (rowList.isEmpty() == false)
//                rowList.removeLast();
//            treeCommit = new QTreeWidgetItem(ui->revList);
//            rowList.append(treeCommit);
//        }
//        else
//        {

//            if (commit->getCommitType() == Commit::BRANCH_COMMIT)
//            {
//                if (rowList.isEmpty() == false)
//                    rowList.removeLast();
//                treeCommit = new QTreeWidgetItem(rowList.at(rowList.count() - 1));
//            }

//            if (commit->getCommitType() == Commit::BRANCH_MERGE_COMMIT)
//            {
//                treeCommit = new QTreeWidgetItem(rowList.at(rowList.count() - 1));
//                if (rowList.isEmpty() == false)
//                    rowList.removeLast();
//                rowList.append (treeCommit);
//            }

//            if (commit->getCommitType() == Commit::MERGE_COMMIT)
//            {
//                treeCommit = new QTreeWidgetItem(rowList.at(rowList.count() - 1));
//                rowList.append (treeCommit);
//            }

//            if (commit->getCommitType() == Commit::NORMAL_COMMIT)
//            {
//                if (rowList.isEmpty() == false && rowList.count() > commit->getRow())
//                {
//                    treeCommit = new QTreeWidgetItem(rowList.at(commit->getRow() - 1));
//                    rowList.removeAt(commit->getRow() - 1);
//                    rowList.insert(commit->getRow()- 1, treeCommit);
//                }
//                else if (rowList.isEmpty() == false)
//                {
//                    treeCommit = new QTreeWidgetItem(rowList.at(rowList.count() -1));
//                    rowList.removeLast();
//                    rowList.append(treeCommit);
//                }
//            }
//        }

//        // set short message
//        treeCommit->setText(1, QString::number(commit->getRow()) + commit->getCommit().shortMessage());

//        // set author
//        treeCommit->setText(2, commit->getCommit().author().name());

//        //set author date
//        treeCommit->setText(3, commit->getCommit().author().when().toString());

//        treeCommit->setExpanded(true);

//    }

}


void MainWindow::on_actionOpen_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::home().path());
    repo = new acRepo(folderName + "/.git");

    connect (repo, SIGNAL(repoOpened()),this, SLOT(loadRepo()));
   // revWalk();
    findAllBranches();
}

/**
 * @brief MainWindow::on_revListTable_cellClicked - A Cell has been clicked so
 * highlight the row it is on
 * @param row - row to highlight
 * @param column - column which has be click we doing use
 */
void MainWindow::on_revListTable_cellClicked(int row, int column)
{
   // ui->revListTable->selectRow(row);
}
