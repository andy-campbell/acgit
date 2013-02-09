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
}

MainWindow::~MainWindow()
{
    delete ui;
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
