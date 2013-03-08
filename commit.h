#ifndef COMMIT_H
#define COMMIT_H

#include "../libqgit2/qgit2.h"

class Commit
{
public:
    enum CommitType
    {
        NO_COMMIT,
        NORMAL_COMMIT,
        BRANCH_COMMIT,
        MERGE_COMMIT,
        BRANCH_MERGE_COMMIT
    };

    Commit(LibQGit2::QGitCommit commit, enum CommitType type, int row , int branchRow, int maxRows);

    virtual ~Commit();

    int getNumParents();

    void setNumParents(int parentsCount = 1);

    int getRow();

    LibQGit2::QGitCommit getCommit() const;

    enum CommitType getCommitType();

    int getMaxRow();

    int getBranchRow();

private:
    LibQGit2::QGitCommit _commit;
    int _row;
    int _branchRow;
    enum CommitType _type;
    int _numParents;
    int maxRow;


};

#endif // COMMIT_H
