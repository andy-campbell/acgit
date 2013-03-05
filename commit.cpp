#include "commit.h"



Commit::Commit(LibQGit2::QGitCommit commit, Commit::CommitType type, int row, int branchRow, int maxRows)
    : _commit(commit), _type (type), _row(row), _branchRow(branchRow), maxRow(maxRows)
{
}

Commit::~Commit()
{
}

int Commit::getNumParents()
{
    return _numParents;
}

void Commit::setNumParents(int parentsCount)
{
    _numParents = parentsCount;
}

int Commit::getRow()
{
    return _row;
}

LibQGit2::QGitCommit Commit::getCommit() const
{
    return _commit;
}

Commit::CommitType Commit::getCommitType()
{
    return _type;
}

int Commit::getMaxRow()
{
    return maxRow;
}

int Commit::getBranchRow()
{
    return _branchRow;
}



