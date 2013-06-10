#include "fileschangesview.h"

filesChangesView::filesChangesView(MainWindow *_mainWindow, diffTextEdit *_selectedCommitDiff, QTreeView *_fileChanged )
    : mainWindow(_mainWindow), selectedCommitDiff(_selectedCommitDiff), filesChanged(_fileChanged)
{
    model = new QStandardItemModel(filesChanged);
    model->setColumnCount(1);

    setupHeader();

    setupModel();

    filesChanged->show();
}

void filesChangesView::clearOldChangedFilesList()
{
    model->removeRows(0, model->rowCount());
}

void filesChangesView::addNewChangedFileList(QStringList changedFileList)
{
    int row = 0;
    foreach (QString file, changedFileList)
    {
        //create items and then add it to the model
        QStandardItem *index = new QStandardItem();
        index->setData(file, Qt::DisplayRole);
        model->setItem(row, 0, index);

        row++;
    }

}

void filesChangesView::updateFileDiff(currentDiff *shownCommit, QString fileName)
{
    selectedCommitDiff->clear();
    selectedCommitDiff->append(shownCommit->getDetaForFile(fileName));
}

void filesChangesView::updateChangedFiles(QStringList changedFileList)
{
    clearOldChangedFilesList();

    addNewChangedFileList(changedFileList);

    // call to use updated model
    setupModel ();

    QModelIndex firstItem = model->index(0,0);
    filesChanged->setCurrentIndex(firstItem);

}

void filesChangesView::update(currentDiff *shownCommit)
{
    QStringList changedFileList = shownCommit->getFileList();
    updateChangedFiles(changedFileList);

    updateFileDiff(shownCommit, changedFileList.first());
}

void filesChangesView::setupHeader()
{
    QStandardItem *header = new QStandardItem();
    header->setData("Files Changed", Qt::DisplayRole);
    model->setHorizontalHeaderItem(0, header);
}

void filesChangesView::setupModel()
{
    filesChanged->setModel(model);
}




