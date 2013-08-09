#include "mainwindow.h"
#include "mainwindowrevview.h"
#include "revviewdelegate.h"

#include <QDebug>

MainWindowRevView::MainWindowRevView(MainWindow *_mainWindow, QTreeView *_revView)
    : mainWindow(_mainWindow), revView(_revView)
{
    model = new QStandardItemModel(this);
    model->setColumnCount(columnCount);

    setupRevViewHeaders();

    setRevViewModel();

    setColumnWidth();

    setupSelectionChangedCallBack();

    revView->show();
}

MainWindowRevView::~MainWindowRevView()
{

}

void MainWindowRevView::setupSelectionChangedCallBack()
{
    connect(revView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(revViewSelectionChanged(QItemSelection,QItemSelection)));
}

void MainWindowRevView::setupDelegate(AcGit::Repository *repo)
{
    revViewDelegate *delegate = new revViewDelegate(repo, this);
    revView->setItemDelegate (delegate);
}

void MainWindowRevView::setRevViewModel ()
{
    revView->setModel(model);
}

void MainWindowRevView::setColumnWidth()
{
    revView->setColumnWidth(1, columnSize);
}

void MainWindowRevView::setupRevViewHeaders()
{
    setHeaderName ("Rev", REV_INDEX);

    setHeaderName ("Short Log", SHORT_LOG_INDEX);

    setHeaderName ("Author", AUTHOR_INDEX);

    setHeaderName ("Date", DATE_INDEX);
}

void MainWindowRevView::clearCommits()
{
    model->clear();
}

void MainWindowRevView::setHeaderName (QString name, int index)
{
    QStandardItem *header = new QStandardItem();
    header->setData(name, Qt::DisplayRole);
    model->setHorizontalHeaderItem(index, header);
}

void MainWindowRevView::addColumnData (int row, int column, QString columnContents)
{
    QStandardItem *entry = new QStandardItem();
    entry->setData(columnContents, Qt::DisplayRole);
    model->setItem(row, column, entry);
}

void MainWindowRevView::addWorkingDirectoryCommit()
{
    addColumnData (working_dir_row, REV_INDEX, "");

    addColumnData (working_dir_row, SHORT_LOG_INDEX, "Working Directory");
}

void MainWindowRevView::addCommit (AcGit::Commit *commit, int row)
{
    addColumnData (row, REV_INDEX, "");

    addColumnData (row, SHORT_LOG_INDEX, commit->shortLog());

    addColumnData (row, AUTHOR_INDEX, commit->author());

    addColumnData (row, DATE_INDEX, commit->dateCreated().toString());
}

void MainWindowRevView::addCommitsToView(AcGit::Repository *repo)
{
    int row = 1;
    addWorkingDirectoryCommit();

    AcGit::ICommits *commitsAgent = repo->CommitsAgent();
    QList<AcGit::Commit *> *commitsList = commitsAgent->getAllCommits();

    foreach (AcGit::Commit* commit, *commitsList)
    {
        addCommit(commit, row);
        row++;
    }
}

void MainWindowRevView::revViewSelectionChanged(QItemSelection selected,QItemSelection deSelected)
{
    QModelIndex index = revView->currentIndex();
    mainWindow->setShownCommit(index.row());
}
