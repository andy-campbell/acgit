#include <QDebug>
#include <QMessageBox>
#include <QDir>

#include "externaldiff.h"

ExternalDiff::ExternalDiff(currentDiff *shownCommit, QString diffExecutable, QString pathToFile)
{
    this->diffExecutable = diffExecutable;
    this->repo = repo;

    writeFileBlobs(shownCommit, pathToFile);

    runExternalDiff();

    cleanupFileBlobs();

}

ExternalDiff::~ExternalDiff()
{
    delete commitTmp;
    delete preComitTmp;
}

void ExternalDiff::writeFileBlobs(currentDiff *shownCommit, QString pathToFile)
{
    AcGit::Commit *commit = shownCommit->getCurrentSelectedCommit();
    AcGit::Tree *tree = commit->tree();

    AcGit::TreeEntry *entry = tree->getEntry(pathToFile);
    if (entry == nullptr)
    {
        qDebug() << "entry is null";
        return;
    }
    AcGit::Blob *blob = entry->fileBlob();
    QString pathToCommit = QDir::tempPath() + "/" + pathToFile + "." + commit->toString();
    qDebug() << pathToCommit;
    commitTmp = new QTemporaryFile(pathToCommit);
    commitTmp->open();

    commitTmp->write(blob->contents().toUtf8());
    commitTmp->close();
    delete blob;
    commit = shownCommit->getCommitDiffedFrom();
    tree = commit->tree();

    entry = tree->getEntry(pathToFile);
    if (entry == nullptr)
    {
        qDebug() << "entry is null";
        return;
    }
    blob = entry->fileBlob();
    QString pathFromCommit = QDir::tempPath() + "/" + pathToFile + "." + commit->toString();
    preComitTmp = new QTemporaryFile(pathFromCommit);
    preComitTmp->open();

    preComitTmp->write(blob->contents().toUtf8());
    preComitTmp->close();
    delete blob;

}

void ExternalDiff::runExternalDiff()
{
    externalProcess = new QProcess(this);
    connect(externalProcess, SIGNAL(error(QProcess::ProcessError)),
                          this, SLOT(processError(QProcess::ProcessError)));

    //pass the name of executable that you want to launch. i have installed helloworld so passed helloworld.
//    externalProcess.arguments()
    QStringList arguments;
    arguments << preComitTmp->fileName() << commitTmp->fileName();
    qDebug() << arguments;
//    externalProcess->
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
