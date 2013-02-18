#ifndef CURRENTCOMMIT_H
#define CURRENTCOMMIT_H

#include "../libqgit2/qgit2.h"

#include <QStringList>

#include "commit.h"
#include "acrepo.h"

class currentCommit
{
public:
    currentCommit(const acRepo *repo, const Commit *from, const Commit *to);

    virtual ~currentCommit();

    /**
     * @brief getFileList gets a list of files changed between
     * the from commit to the to commit.
     * @return The list of files
     */
    QStringList getFileList();

    /**
     * @brief getDetaForFile this function check if the filename
     * is in the filelist. If it is not then it returns an empty string
     * otherwise it will get the changes for the file between the
     * two commits.
     * @param filename The file to get the delta for
     * @return  The delta of the file
     */
    QString getDetaForFile(QString &filename);

    /**
     * @brief getCurrentSelectedCommit this function returns the "to" commit
     * @return return the "to" commit which is the current selected commit
     */
    const Commit *getCurrentSelectedCommit();

private:
    const Commit *_selectedCommit;
    LibQGit2::QGitDiff *diff;
};

#endif // CURRENTCOMMIT_H
