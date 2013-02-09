#include "commit.h"



Commit::Commit(LibQGit2::QGitCommit commit, Commit::CommitType type, int row)
    : _commit(commit), _type (type), _row(row)
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

LibQGit2::QGitCommit Commit::getCommit()
{
    return _commit;
}

Commit::CommitType Commit::getCommitType()
{
    return _type;
}



