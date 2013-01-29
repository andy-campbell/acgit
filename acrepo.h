#ifndef ACREPO_H
#define ACREPO_H

#include <QString>
#include <QObject>

#include "../libqgit2/qgit2.h"
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

    void setRepo(const LibQGit2::QGitRepository & value);

signals:
    void repoOpened();

private:
    LibQGit2::QGitRepository repo;
};

#endif // ACREPO_H
