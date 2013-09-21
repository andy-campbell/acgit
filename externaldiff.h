#ifndef EXTERNALDIFF_H
#define EXTERNALDIFF_H

#include <QProcess>
#include <QObject>
#include <QTemporaryFile>

#include "currentdiff.h"
#include "libacgit_global.h"

#define MAX_DIFFABLE 3

class ExternalDiff : public QObject
{
    Q_OBJECT
public:
    ExternalDiff(currentDiff *showCommit, QString diffExecutable, QString pathToFile, AcGit::Repository *repo);

    ~ExternalDiff();
private slots:
    void processError(QProcess::ProcessError err);

private:
    QProcess *externalProcess;
    QString diffExecutable;
    QTemporaryFile *commitTmp[MAX_DIFFABLE];
    AcGit::Repository *repo;
    int tmpFilesCreated;

    void cleanupFileBlobs();
    void runExternalDiff();
    void writeFileBlobs(currentDiff *shownCommit, QString pathToFile);
};

#endif // EXTERNALDIFF_H
