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

#ifndef COMMIT_H
#define COMMIT_H

#include "../libqgit2/qgit2.h"

#include <QVector>

class Commit
{
public:
    enum CommitType
    {
        NO_COMMIT,
        NO_COMMIT_H,
        NO_COMMIT_WORKING_DIR,
        EMPTY_LANE,
        EMPTY_LANE_H,
        NORMAL_COMMIT,
        NORMAL_COMMIT_H,
        BRANCH_COMMIT,
        BRANCH_COMMIT_H,
        BRANCH_COMMIT_UP,
        BRANCH_COMMIT_UP_H,
        MERGE_COMMIT,
        MERGE_COMMIT_H,
        MERGE_COMMIT_DOWN,
        MERGE_COMMIT_DOWN_H,
        BRANCH_MERGE_COMMIT,
        BRANCH_MERGE_COMMIT_DOWN,
        BRANCH_MERGE_COMMIT_DOWN_H,
        BRANCH_MERGE_COMMIT_UP,
        BRANCH_MERGE_COMMIT_UP_H,
        BRANCH_MERGE_COMMIT_BOTH,
        BRANCH_MERGE_COMMIT_BOTH_H
    };

    struct Lane
    {
        int activeRow;
        enum CommitType type;
        QVector<enum CommitType> *lanes;
    };

    Commit(LibQGit2::QGitCommit commit, struct Lane laneinfo, int prevMaxRows);

    virtual ~Commit();

    int getNumParents();

    void setNumParents(int parentsCount = 1);

    int getRow();

    LibQGit2::QGitCommit getCommit() const;

    enum CommitType getCommitType() const;

    QVector<enum CommitType>* getCurrentRowState();

    int getMaxRows();
private:
    LibQGit2::QGitCommit _commit;
    enum CommitType _type;
    int _numParents;
    struct Lane _laneInfo;
    int _prevMaxRows;


};

#endif // COMMIT_H
