#ifndef COMMIT_H
#define COMMIT_H

#include "../libqgit2/qgit2.h"

class Commit
{
public:
    enum CommitType
    {
        NORMAL_COMMIT,
        BRANCH_COMMIT,
        MERGE_COMMIT,
        BRANCH_MERGE_COMMIT
    };

    Commit(LibQGit2::QGitCommit commit, enum CommitType type, int row );

    virtual ~Commit();

    int getNumParents();

    void setNumParents(int parentsCount = 1);

    int getRow();

    LibQGit2::QGitCommit getCommit() const;

    enum CommitType getCommitType();

private:
    LibQGit2::QGitCommit _commit;
    int _row;
    enum CommitType _type;
    int _numParents;


};

#endif // COMMIT_H
