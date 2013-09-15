#ifndef EXTERNALDIFF_H
#define EXTERNALDIFF_H

#include <QProcess>
#include <QObject>
#include <QTemporaryFile>

#include "currentdiff.h"
#include "libacgit_global.h"

class ExternalDiff : public QObject
{
    Q_OBJECT
public:
    ExternalDiff(currentDiff *showCommit, QString diffExecutable, QString pathToFile);

    ~ExternalDiff();
private slots:
    void processError(QProcess::ProcessError err);

private:
    QProcess *externalProcess;
    QString diffExecutable;
    QTemporaryFile *preComitTmp;
    QTemporaryFile *commitTmp;
    AcGit::Repository *repo;

    void cleanupFileBlobs();
    void runExternalDiff();
    void writeFileBlobs(currentDiff *shownCommit, QString pathToFile);
};

#endif // EXTERNALDIFF_H
