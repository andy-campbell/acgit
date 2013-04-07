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

#include "acrepo.h"
#include "commit.h"


#include <QDebug>

acRepo::acRepo(QString directory)
{
    repo.open(directory);

    // check if there is any changes to the working directory
    LibQGit2::QGitDiff *workingDirChanges = new LibQGit2::QGitDiff (repo);

    // if there are changes to working dir create a dummy commit
    if (workingDirChanges->diffWorkingDir())
    {
        QVector<enum Commit::CommitType> *currentRowState = new QVector<enum Commit::CommitType>;
        currentRowState->append(Commit::NO_COMMIT_WORKING_DIR);
        struct Commit::Lane lane;

        lane.activeRow = -1;
        lane.lanes = currentRowState;
        lane.type = Commit::NO_COMMIT_WORKING_DIR;

        // empty commit with type being lane having no_commit_working_dir
        Commit* workDir = new Commit(LibQGit2::QGitCommit(), lane, 1);

        commits.append(workDir);
        commitLookup.insert(0, commits.size() - 1);
    }

    // set walk commit to head
    LibQGit2::QGitCommit walkCommit = repo.lookupCommit(repo.head().oid());
    LibQGit2::QGitRevWalk walker(repo);

    QVector<LibQGit2::QGitOId> nextCommits;
    nextCommits.reserve(400);

    nextCommits.insert(0, repo.head().oid());
    Commit* newCommit = populateCommit(walkCommit, nextCommits, 1);

    commits.append (newCommit);
    commitLookup.insert(newCommit->getCommit().oid().format(), commits.size() - 1);

    walker.setSorting(LibQGit2::QGitRevWalk::Topological);

    walker.push(walkCommit);
    walker.next(walkCommit);

    while (walker.next(walkCommit) == true)
    {
        newCommit = populateCommit(walkCommit, nextCommits, newCommit->getMaxRows());
        commits.append(newCommit);
        commitLookup.insert(newCommit->getCommit().oid().format(), commits.size() - 1);
    }

    // add branches to appropriate commits
    foreach (QString branch, getBranches())
    {
        int index = lookupBranch(branch);
        commits[index]->addBranch(branch);
    }

    // add tags to appropriate commits
    foreach (QString tag, getTags())
    {
        int index = lookupTags(tag);
        commits[index]->addTag(tag);
    }


    // emit that the repo has been opened.
    emit repoOpened ();

}

acRepo::~acRepo()
{
}

LibQGit2::QGitRepository acRepo::getRepo() const
{
    return repo;
}

void acRepo::setRepo(const LibQGit2::QGitRepository & value)
{
    repo = value;
}

void acRepo::addCommit(const LibQGit2::QGitCommit &commit)
{
    //commits.append(commit);
}

QVector<Commit*> acRepo::getAllCommits()
{
    return commits;
    //return Lanes;
}

QStringList acRepo::getBranches()
{
    QStringList allBranches;
    allBranches = repo.showAllBranches (LibQGit2::QGitRepository::QGIT_LOCAL);
    allBranches.append(repo.showAllBranches (LibQGit2::QGitRepository::QGIT_REMOTE));
    allBranches.removeDuplicates();

    return allBranches;
}

/**
 * @brief acRepo::lookupTag Takes a tag name and returns the index in the
 * commit vector or -1 if the tag name was not found
 * @param tagName - Tag to lookup
 * @return return index of tagged commit in vector or -1
 */
int acRepo::lookupTag(QString tagName)
{
    int commitIndex = 0;
    try
    {
            LibQGit2::QGitRef ref = repo.lookupRef("refs/tags/" + tagName);

            // search known commits first
            QMap<QByteArray, int>::Iterator itor = commitLookup.find(ref.oid().format());

            if (itor == commitLookup.end())
            {
                // no luck finding it so far so do a tag lookup
                // incase the tag has its own oid seperate to the commit oid
                LibQGit2::QGitTag tag = repo.lookupTag(ref.oid());

                LibQGit2::QGitObject obj = tag.target();

                itor = commitLookup.find(obj.oid().format());
            }

            // check if itor has found it.
            if (itor == commitLookup.end())
            {
                // just not our day for finding this tag so return -1
                return -1;
            }

            return itor.value();
    }
    catch (LibQGit2::QGitException e)
    {
        qDebug() << "no commit with that commit is found with tagName";
        qDebug() << e.message();
    }

    return commitIndex;
}

int acRepo::lookupBranch(QString branchName)
{
    int commitIndex = -1;
    try
    {
            LibQGit2::QGitRef ref = repo.lookupRef("refs/remotes/" + branchName);

            // search known commits first
            QMap<QByteArray, int>::Iterator itor = commitLookup.find(ref.oid().format());

            // check if itor has found it.
            if (itor == commitLookup.end())
            {
                // just not our day for finding this tag so return -1
                return -1;
            }

            return itor.value();
    }
    catch (LibQGit2::QGitException e)
    {
        // most likely we are here because reference could not be found
        try
        {
            LibQGit2::QGitRef ref = repo.lookupRef("refs/heads/" + branchName);

            // search known commits first
            QMap<QByteArray, int>::Iterator itor = commitLookup.find(ref.oid().format());

            if (itor == commitLookup.end())
            {
                // just not our day for finding this tag so return -1
                return -1;
            }

            return itor.value();

        }
        catch (LibQGit2::QGitException e2)
        {
            qDebug() << "no commit with that commit is found with branchName";
            qDebug() << e2.message();
        }

    }

    return commitIndex;
}

QStringList acRepo::getTags()
{
    return repo.listTags();
}


//TODO comment me
QVector<enum Commit::CommitType> *buildUpLane(QVector<int> branchedToRow, QVector<int> mergeFromRow,
                                              QVector<LibQGit2::QGitOId> &nextCommits, int activeRow,
                                              enum Commit::CommitType activeRowType, int curMaxRows, int prevMaxRow)
{
    QVector<enum Commit::CommitType> *currentRowState = new QVector<enum Commit::CommitType>;
    bool handledLastMerge = false;
    bool handledLastBranch = false;
    int handledMerges = 0;
    int handledBranched = 0;

    for (int i = 0; i < prevMaxRow; i++)
    {
        // anything before active row is an empty lane or non commit lane
        if (i < activeRow )
        {
            LibQGit2::QGitOId zero;
            if (nextCommits[i] == zero)
            {
                currentRowState->append(Commit::EMPTY_LANE);
                continue;
            }
            else
            {
                currentRowState->append(Commit::NO_COMMIT);
                continue;
            }
        }
        // we have worked out the lane type so no work needs to be done
        else if (i == activeRow)
        {
            currentRowState->append(activeRowType);
            continue;
        }

        if (activeRowType == Commit::BRANCH_COMMIT)
        {
            if (branchedToRow.contains(i))
            {
                handledBranched++;
                if (handledBranched == branchedToRow.size())
                {
                    currentRowState->append(Commit::BRANCH_COMMIT_UP);
                    handledLastBranch = true;
                }
                else
                {
                    currentRowState->append(Commit::BRANCH_COMMIT_UP_H);
                }

            }
            else
            {
                LibQGit2::QGitOId zero;
                if (nextCommits[i] == zero)
                {
                    if (handledLastBranch)
                        currentRowState->append(Commit::EMPTY_LANE);
                    else
                        currentRowState->append(Commit::EMPTY_LANE_H);
                }
                else
                {
                    if (handledLastBranch)
                        currentRowState->append(Commit::NO_COMMIT);
                    else
                        currentRowState->append(Commit::NO_COMMIT_H);

                }
            }
            continue;
        }

        if (activeRowType == Commit::MERGE_COMMIT)
        {
            // if last lane then type is merge down
            if (mergeFromRow.contains(i))
            {
                handledMerges++;
                // -1 for the active row
                if (handledMerges == mergeFromRow.size() - 1)
                {
                    currentRowState->append(Commit::MERGE_COMMIT_DOWN);
                    handledLastMerge = true;
                }
                else
                {
                    currentRowState->append(Commit::MERGE_COMMIT_DOWN_H);
                }
            }
            else
            {
                LibQGit2::QGitOId zero;
                if (nextCommits[i] == zero)
                {
                    if (handledLastMerge)
                        currentRowState->append(Commit::EMPTY_LANE);
                    else
                        currentRowState->append(Commit::EMPTY_LANE_H);
                }
                else
                {
                    if (handledLastMerge)
                        currentRowState->append(Commit::NO_COMMIT);
                    else
                        currentRowState->append(Commit::NO_COMMIT_H);
                }
            }
            continue;
        }

        if (activeRowType == Commit::BRANCH_MERGE_COMMIT)
        {
            if (mergeFromRow.contains(i) && branchedToRow.contains(i))
            {
                handledMerges++;
                handledBranched++;

                if (handledMerges == mergeFromRow.size() -1)
                    handledLastMerge = true;

                if (handledBranched == branchedToRow.size())
                    handledLastBranch = true;

                if (handledBranched && handledLastBranch)
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_BOTH);
                else
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_BOTH_H);

            }
            else if (mergeFromRow.contains(i))
            {
                handledMerges++;
                if (handledLastBranch && handledMerges == mergeFromRow.size() -1)
                {
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_DOWN);
                    handledLastMerge = true;
                }
                else
                {
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_DOWN_H);
                }

            }
            else if (branchedToRow.contains(i))
            {
                handledBranched++;
                if (handledLastMerge && handledBranched == branchedToRow.size())
                {
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_UP);
                    handledLastBranch = true;
                }
                else
                {
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_UP_H);
                }
            }
            else
            {
                LibQGit2::QGitOId zero;
                if (nextCommits[i] == zero)
                {
                    if (handledLastMerge && handledLastBranch)
                        currentRowState->append(Commit::EMPTY_LANE);
                    else
                        currentRowState->append(Commit::EMPTY_LANE_H);
                }
                else
                {
                    if (handledLastMerge && handledLastBranch)
                        currentRowState->append(Commit::NO_COMMIT);
                    else
                        currentRowState->append(Commit::NO_COMMIT_H);

                    if (i == prevMaxRow -1 && i == branchedToRow.last() -1)
                    {
                        currentRowState->append(Commit::BRANCH_MERGE_COMMIT_UP);
                    }
                }
            }
            continue;
        }

        if (activeRowType == Commit::NORMAL_COMMIT)
        {
            if (i > activeRow)
            {
                LibQGit2::QGitOId zero;
                if (nextCommits[i] == zero)
                {
                    currentRowState->append(Commit::EMPTY_LANE);
                }
                else
                {
                    currentRowState->append(Commit::NO_COMMIT);
                }
            }

        }

    }

    // there a case on the branch or merge/branch where there is more items to
    // be added to currentRowState eg. mergeUp if last or lines in between.
    if (curMaxRows > currentRowState->size())
    {
        int diff = curMaxRows - currentRowState->size();
        int numRows = currentRowState->size();

        for (int i = 0; i < diff; i++)
        {
            if (branchedToRow.contains(numRows + i))
            {
                handledBranched++;
                if (activeRowType == Commit::BRANCH_MERGE_COMMIT)
                {
                    if (handledBranched == branchedToRow.size())
                    {
                        currentRowState->append(Commit::BRANCH_MERGE_COMMIT_UP);
                        handledLastBranch = true;
                    }
                    else
                    {
                        currentRowState->append(Commit::BRANCH_MERGE_COMMIT_UP_H);
                    }
                }
                else if (activeRowType == Commit::BRANCH_COMMIT)
                {
                    if (handledBranched == branchedToRow.size())
                    {
                        currentRowState->append(Commit::BRANCH_COMMIT_UP);
                        handledLastBranch = true;
                    }
                    else
                    {
                        currentRowState->append(Commit::BRANCH_COMMIT_UP_H);
                    }
                }
            }
            else
            {
                currentRowState->append(Commit::EMPTY_LANE_H);
            }
        }
    }
    return currentRowState;
}

// TODO refactor this function it is far too long!!
Commit* acRepo::populateCommit(LibQGit2::QGitCommit &commit, QVector<LibQGit2::QGitOId> &nextCommits, int prevMaxRow)
{
    enum Commit::CommitType type = Commit::NO_COMMIT;
    int count = 0;
    bool mergeCommit = false;
    QVector<int> branchedToRow;
    QVector<int> mergeFromRow;

    int activeRow = -1;
    int curMaxRows = prevMaxRow;

    // count the next commits that have the same oid as the commit
    for (int nextCommitIndex = 0; nextCommitIndex < nextCommits.size(); nextCommitIndex++)
    {
        LibQGit2::QGitOId nextCommit = nextCommits.at(nextCommitIndex);
        if (nextCommit == commit.oid())
        {
            // increament count of children with this commit
            if (count == 0)
            {
                activeRow = nextCommitIndex;
            }
            else
            {
                branchedToRow.append(nextCommitIndex);
            }
            count++;
            //reset commit to zero
            LibQGit2::QGitOId zero;
            nextCommits[nextCommitIndex] = zero;
        }
    }

    // Loops over parents of this commit and add to nextCommits List.
    for (uint i = 0; i < commit.parentCount() && commit.parentCount() > 1; i++)
    {
        bool added = false;
        for (int k = activeRow; k < nextCommits.size(); k++)
        {
            //default value of gitoid is 0
            LibQGit2::QGitOId zero;
            if (nextCommits[k] == zero)
            {
                nextCommits[k] = commit.parent(i).oid();
                added = true;
                mergeFromRow.append(k);
                break;
            }
        }
        if (added == false)
        {
            nextCommits.append(commit.parent(i).oid());
            mergeFromRow.append(nextCommits.size() - 1);
        }

        // more than one parent commit so must be a merge of some sort.
        mergeCommit = true;
    }

    if (mergeCommit && count == 1)
    {
        type = Commit::MERGE_COMMIT;
    }

    // if merge commit and child count > 1 then we have a merge commit
    if (mergeCommit && count > 1)
    {
        type = Commit::BRANCH_MERGE_COMMIT;
    }

    // child count is more than 1 so we have a branch
    if (!mergeCommit && count > 1)
    {
        // remove current commits that match commit
        type = Commit::BRANCH_COMMIT;

        // make sure this isn't the last commit
        if (commit.parentCount() != 0)
        {
            for (int k = activeRow; k < nextCommits.size(); k++)
            {
                LibQGit2::QGitOId zero;
                if (nextCommits[k] == zero)
                {
                    if (k < nextCommits.size())
                    {
                        nextCommits[k] = commit.parent(0).oid();;
                    }
                    else
                    {
                        nextCommits.insert(k, commit.parent(0).oid());
                    }
                    break;
                }
            }
        }
    }

    // only one parent and only one child means this must be a commits
    if (!mergeCommit && count == 1)
    {
        type = Commit::NORMAL_COMMIT;
        if (commit.parentCount() != 0)
        {
             nextCommits[activeRow] = commit.parent(0).oid();
        }
    }

    // an error has occured dump info so it can be debugged
    if (count == 0)
    {
        qDebug() << "commit oid = " << commit.oid().format();
        int a = 0;
        foreach (LibQGit2::QGitOId oid, nextCommits)
        {
            qDebug() << "next commit in lane " << a << " oid = " << oid.format();
            a++;
        }

        qDebug() << "I found an orphan node";
        return nullptr;
    }

    // find last Row as size of vector is not accurate as it may have empty commits
    for (int i = nextCommits.size() - 1; i >= 0; i--)
    {
        LibQGit2::QGitOId zero;
        if (nextCommits[i] != zero)
        {
            prevMaxRow = i + 1;
            break;
        }
    }

    Commit::Lane lane;
    lane.activeRow = activeRow;
    lane.lanes = buildUpLane(branchedToRow, mergeFromRow, nextCommits, activeRow,type, curMaxRows, prevMaxRow);
    lane.type = type;

    Commit* newCommit = new Commit (commit, lane, prevMaxRow);

    return newCommit;
}
