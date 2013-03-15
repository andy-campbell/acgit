/**
 * This file is part of AcGit.
 *
 * AcGit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AcGit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */


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

