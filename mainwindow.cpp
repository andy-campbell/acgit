#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../libqgit2/qgit2.h"

#include "acrepo.h"


#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

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
 * @brief MainWindow::revWalk walk the full repo
 * TODO make this less static
 */
void MainWindow::revWalk ()
{
    if (repo == NULL)
    {
        QMessageBox msgBox;
        msgBox.setText("No git repository selected");
        msgBox.setInformativeText("Please open one!");
        int ret = msgBox.exec();

        return;
    }
    // set walk commit to head
    LibQGit2::QGitCommit walkCommit = repo->getRepo().lookupCommit(repo->getRepo().head().oid());
    LibQGit2::QGitRevWalk walker(repo->getRepo());

    walker.setSorting(LibQGit2::QGitRevWalk::Topological);

    walker.push(walkCommit);

    int count = 0;
    qDebug() << "got to here";
    while (walker.next(walkCommit) == true)
    {
        ui->revListTable->insertRow(count);
        // set short message
        QTableWidgetItem* shortMessage = new QTableWidgetItem(walkCommit.shortMessage());

        ui->revListTable->setItem(count, 1, shortMessage);
        // set author
        QTableWidgetItem* author = new QTableWidgetItem(walkCommit.author().name());

        ui->revListTable->setItem(count, 2, author);

        //set author date
        QTableWidgetItem* date = new QTableWidgetItem(walkCommit.author().when().toString());

        ui->revListTable->setItem(count, 3, date);
        //ui->textEdit->append (walkCommit.author().name() + "->" + walkCommit.message());

        // increase row count
        count ++;
    }

}

void MainWindow::loadRepo()
{
    qDebug() << "loadRepo called";
    revWalk();
}

void MainWindow::findAllBranches()
{
    QStringList branches = repo->getRepo().showAllBranches();

    foreach (QString branch, branches)
    {
        ui->diffView->append(branch + "\n");
    }

//    if (head)
//    {
//        repo->getRepo().
//    }

}


void MainWindow::on_actionOpen_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::home().path());
    repo = new acRepo(folderName + "/.git");

    qDebug() << folderName + "/.git";

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
    ui->revListTable->selectRow(row);
}
