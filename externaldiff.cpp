#include <QDebug>
#include <QMessageBox>
#include <QDir>

#include "externaldiff.h"

ExternalDiff::ExternalDiff(currentDiff *shownCommit, QString diffExecutable, QString pathToFile, AcGit::Repository *repo)
{
    this->diffExecutable = diffExecutable;
    this->repo = repo;
    tmpFilesCreated = 0;

    writeFileBlobs(shownCommit, pathToFile);

    runExternalDiff();

    cleanupFileBlobs();

}

ExternalDiff::~ExternalDiff()
{
    for(int index = 0; index < tmpFilesCreated; index++)
    {
        delete commitTmp[index];
    }
}

void ExternalDiff::writeFileBlobs(currentDiff *shownCommit, QString pathToFile)
{
    AcGit::Commit *commit = shownCommit->getCurrentSelectedCommit();
    AcGit::Tree *tree = commit->tree();
    int parentCount = commit->parentCount();
    if(parentCount > MAX_DIFFABLE)
    {
        return;
        // throw error message to user.
    }

    AcGit::TreeEntry *entry = tree->getEntry(pathToFile);
    if (entry == nullptr)
    {
        qDebug() << "entry is null";
        return;
    }
    qDebug() << entry->fullPath();
    AcGit::Blob *blob = entry->fileBlob();
    QString pathToCommit = QDir::tempPath() + "/" + entry->fileName() + "." + commit->toString();
    commitTmp[0] = new QTemporaryFile(pathToCommit);
    commitTmp[0]->open();

    commitTmp[0]->write(blob->contents().toUtf8());
    commitTmp[0]->close();
    tmpFilesCreated++;
    delete blob;

    qDebug() << parentCount;
    for(int index = 0; index < parentCount; index++)
    {
        AcGit::ICommits *commitAgent = repo->CommitsAgent();

        AcGit::Commit *parentCommit = commitAgent->lookupCommit(commit->parentSha(index));
        tree = parentCommit->tree();

        entry = tree->getEntry(pathToFile);
        if (entry == nullptr)
        {
            qDebug() << "entry is null";
            return;
        }
        blob = entry->fileBlob();
        QString pathFromCommit = QDir::tempPath() + "/" + entry->fileName() + "." + parentCommit->toString();

        commitTmp[index + 1] = new QTemporaryFile(pathFromCommit);
        commitTmp[index + 1]->open();
        commitTmp[index + 1]->open();

        commitTmp[index + 1]->write(blob->contents().toUtf8());
        commitTmp[index + 1]->close();
        tmpFilesCreated++;
        delete blob;


    }
}

void ExternalDiff::runExternalDiff()
{
    externalProcess = new QProcess(this);
    connect(externalProcess, SIGNAL(error(QProcess::ProcessError)),
                          this, SLOT(processError(QProcess::ProcessError)));

    QStringList arguments;
    for(int index = 0; index < tmpFilesCreated; index++)
    {
        arguments << commitTmp[index]->fileName();
    }
    externalProcess->start(diffExecutable, arguments);
}

void ExternalDiff::cleanupFileBlobs()
{

}

void ExternalDiff::processError(QProcess::ProcessError err)
{
    switch(err)
    {
    case QProcess::FailedToStart:
        QMessageBox::information(0,"FailedToStart","FailedToStart");
        break;
    case QProcess::Crashed:
        QMessageBox::information(0,"Crashed","Crashed");
        break;
    case QProcess::Timedout:
        QMessageBox::information(0,"FailedToStart","FailedToStart");
        break;
    case QProcess::WriteError:
        QMessageBox::information(0,"Timedout","Timedout");
        break;
    case QProcess::ReadError:
        QMessageBox::information(0,"ReadError","ReadError");
        break;
    case QProcess::UnknownError:
        QMessageBox::information(0,"UnknownError","UnknownError");
        break;
    default:
        QMessageBox::information(0,"default","default");
        break;
    }
}
