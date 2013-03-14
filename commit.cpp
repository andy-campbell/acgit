#include "commit.h"



Commit::Commit(LibQGit2::QGitCommit commit, struct Lane laneinfo)
    : _commit(commit), _laneInfo (laneinfo)
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
    return _laneInfo.activeRow;
}

LibQGit2::QGitCommit Commit::getCommit() const
{
    return _commit;
}

Commit::CommitType Commit::getCommitType()
{
    return _laneInfo.type;
}

int Commit::getMaxRow()
{
    return _laneInfo.maxRows;
}

QVector<int> Commit::getBranchRow()
{
    return _laneInfo.branchedToRow;
}

QVector<int> Commit::getMergedFromRow()
{
    return _laneInfo.mergeFromRow;
}

QVector<LibQGit2::QGitOId> Commit::getState()
{
    return _laneInfo.currentState;
}

QVector<Commit::CommitType> *Commit::getCurrentRowState()
{
    return _laneInfo.currentRowState;
}



