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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //set column count to 4
    // TODO make change back to 4
    ui->revList->setColumnCount(5);

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
        msgBox.exec();

        return;
    }
    // set walk commit to head
    LibQGit2::QGitCommit walkCommit = repo->getRepo().lookupCommit(repo->getRepo().head().oid());
    LibQGit2::QGitRevWalk walker(repo->getRepo());

    walker.setSorting(LibQGit2::QGitRevWalk::Topological);

    walker.push(walkCommit);

    while (walker.next(walkCommit) == true)
    {
        QTreeWidgetItem *commit = new QTreeWidgetItem(ui->revList);

        // set short message
        commit->setText(1, walkCommit.shortMessage());

        // set author
        commit->setText(2, walkCommit.author().name());

        //set author date
        commit->setText(3, walkCommit.author().when().toString());

        //TODO remove me - sets branch name
//        LibQGit2::QGitRef *ref = new LibQGit2::QGitRef ();

//        try
//        {
//            ref->setOId(walkCommit.oid());
//        }
//        catch (...)
//        {
//            qDebug() << "An expection occured";
//        }

//        commit->setText(4, ref->name());
    }

}

void MainWindow::loadRepo()
{
    revWalk();
}

void MainWindow::findAllBranches()
{
    QStringList branches = repo->getRepo().listReferences();

    foreach (QString branch, branches)
    {
        ui->diffView->append(branch + "\n");
    }
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
