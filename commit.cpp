#include "commit.h"



Commit::Commit(LibQGit2::QGitCommit commit, struct Lane laneinfo, int prevMaxRows)
    : _commit(commit), _laneInfo (laneinfo), _prevMaxRows(prevMaxRows)
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

QVector<Commit::CommitType> *Commit::getCurrentRowState()
{
    return _laneInfo.lanes;
}

int Commit::getMaxRows()
{
    return _prevMaxRows;
}

