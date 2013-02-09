#ifndef ACREPO_H
#define ACREPO_H

#include <QString>
#include <QObject>
#include <QVector>

#include "../libqgit2/qgit2.h"

#include "commit.h"

/**
 * This class handles the calls about the repo
 */
class acRepo : public QObject
{
    Q_OBJECT

public:
    explicit acRepo(QString directory);

    virtual ~acRepo();

    LibQGit2::QGitRepository getRepo();

    void setRepo(const LibQGit2::QGitRepository &value);

    void addCommit (const LibQGit2::QGitCommit &commit);

    QVector<Commit*> getAllCommits();

signals:
    void repoOpened();

private:
    Commit *populateCommit(LibQGit2::QGitCommit &commit, QList<LibQGit2::QGitOId> &nextCommits);
    LibQGit2::QGitRepository repo;
    QVector<Commit*> commits;
};

#endif // ACREPO_H
