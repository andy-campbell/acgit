#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../libqgit2/qgit2.h"

#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    revWalk();
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
    LibQGit2::QGitRepository repo;

    repo.open("/home/andrew/build/libgit2");

    // set walk commit to head
    LibQGit2::QGitCommit walkCommit = repo.lookupCommit(repo.head().oid());
    LibQGit2::QGitRevWalk walker(repo);

    walker.setSorting(LibQGit2::QGitRevWalk::Topological);

    walker.push(walkCommit);

    LibQGit2::QGitSignature sig;

    int count = 0;

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
