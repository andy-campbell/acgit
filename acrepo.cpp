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
    Commit* newCommit = populateCommit(walkCommit, nextCommits, nullptr);

    commits.append (newCommit);
    commitLookup.insert(newCommit->getCommit().oid().format(), commits.size() - 1);

    walker.setSorting(LibQGit2::QGitRevWalk::Topological);

    walker.push(walkCommit);
    walker.next(walkCommit);

    while (walker.next(walkCommit) == true)
    {
        newCommit = populateCommit(walkCommit, nextCommits, newCommit);
        commits.append(newCommit);
        commitLookup.insert(newCommit->getCommit().oid().format(), commits.size() - 1);
    }

    // add branches to appropriate commits
    foreach (QString branch, getBranches())
    {
        int index = lookupBranch(branch);
        if (index != -1)
        {
            commits[index]->addBranch(branch);
        }
    }

    // add tags to appropriate commits
    foreach (QString tag, getTags())
    {
        int index = lookupTag(tag);
        if (index != -1)
        {
            commits[index]->addTag(tag);
        }
    }

    // emit that the repo has been opened.
    emit repoOpened ();

}

acRepo::~acRepo()
{
    foreach (Commit *commit, commits)
    {
        delete commit;
    }
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
    int commitIndex = -1;
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

/**
 * @brief branchMergeAppend This function is used to add branch merge up and
 * branch merge down or their _H varients to currentRowState.
 *
 * @param currentRowState The current state of the commits lanes
 * @param type The base type which is to add be added to currentRowState
 * @param numHandled The number of merges or branches which have been handled
 * @param size The total number of merges or branches which can be handled.
 * @param handledLast Dependant on if the last merge has been added to currrentRowState and
 * and this is called from the branch check and vise versa for the branch case.
 * @return true or false depending on if is the last of the type to be handled.
 */
bool branchMergeAppend (QVector<enum Commit::CommitType> *currentRowState, enum Commit::CommitType type,
                        int &numHandled, int size, bool handledLast)
{
    numHandled++;
    if (handledLast && numHandled == size)
    {
        currentRowState->append(type);
        return true;
    }
    else
    {
        currentRowState->append(static_cast<Commit::CommitType> (type + 1));
    }
    return false;

}

/**
 * @brief handleRowTypeNotEqual This function adds an empty lanes or no commit lane
 * to currentRowState. This is called as part of the dealing with merge commits or
 * branch commits.
 *
 * @param currentRowState The current state of the commits lanes
 * @param emptyCommit This is to be true if and empty lane is to be displayed and
 * false if no commit is to be displayed.
 * @param handledLast True if the last merge or branch has been handled.
 */
void handleRowTypeNotEqual (QVector<enum Commit::CommitType> *currentRowState, bool emptyCommit, bool handledLast)
{
    if (emptyCommit)
    {
        if (handledLast)
            currentRowState->append(Commit::EMPTY_LANE);
        else
            currentRowState->append(Commit::EMPTY_LANE_H);
    }
    else
    {
        if (handledLast)
            currentRowState->append(Commit::NO_COMMIT);
        else
            currentRowState->append(Commit::NO_COMMIT_H);
    }
}

/**
 * @brief appendRowTypeEqual Adds type or type + 1 which is the _H version of type to
 * currentRow State. This function returns true when it is the last of the type to
 * be handled.
 *
 * @param currentRowState The current state of the commits lanes
 * @param type The base type which is to add be added to currentRowState
 * @param numHandled The number of merges or branches which have been handled
 * @param size The total number of merges or branches which can be handled.
 * @return true or false depending on if is the last of the type to be handled.
 */
bool appendRowTypeEqual(QVector<enum Commit::CommitType> *currentRowState, enum Commit::CommitType type,
                        int &numHandled, int size)
{
    numHandled++;
    if (numHandled == size)
    {
        currentRowState->append(type);
        return true;
    }
    else
    {
        // cast type back to an enum of type + 1 which is the _H version of the type given
        currentRowState->append(static_cast<Commit::CommitType> (type + 1));
    }

    return false;
}

/**
 * @brief branchsExtras This function handles the case were the previous commit had more
 * lanes than is currently in the row state. This occurs when there are branches or multiple
 * branches from this commit.
 *
 * @param currentRowState The current state of the commits lanes
 * @param prevMaxRows The number of rows the previous commit contains
 * @param branchedToRow A Set that contains all of the indexs of the branches.
 * @param activeRowType The type of the active row
 * @param handledBranches The number of branches which have been added to the currentRowState
 */
void branchsExtras (QVector<enum Commit::CommitType> *currentRowState, int &prevMaxRows, QSet<int> &branchedToRow,
                    enum Commit::CommitType activeRowType, int handledBranches)
{
    if (currentRowState->size() < prevMaxRows)
    {
        int diff = prevMaxRows - currentRowState->size();
        int numRows = currentRowState->size();

        for (int i = 0; i < diff; i++)
        {
            if (branchedToRow.contains(numRows + i))
            {
                if (activeRowType == Commit::BRANCH_MERGE_COMMIT)
                {
                    appendRowTypeEqual (currentRowState, Commit::BRANCH_MERGE_COMMIT_UP,
                                                            handledBranches, branchedToRow.size());
                }
                else if (activeRowType == Commit::BRANCH_COMMIT)
                {
                    appendRowTypeEqual (currentRowState, Commit::BRANCH_COMMIT_UP,
                                                            handledBranches, branchedToRow.size());
                }
            }
            else
            {
                currentRowState->append(Commit::EMPTY_LANE_H);
            }
        }
    }
}


/**
 * @brief buildUpLane This function builds up a QVector with for a specific commit containing what should
 * be displayed in the Rev view column.
 *
 * @param branchedToRow This is a set of branches that the commit contains. This information is used when the
 * active row type is Branch_Commit or Merge_Branch_Commit.
 * @param mergeFromRow This is a set of merges that the commit contains. This information is used when the
 * active row type Merge_Commit or Merge_Branch_Commit
 * @param nextCommits - A Vector containing a list of the next oid to match against
 * @param activeRow The active row number
 * @param activeRowType The type of the active row This can be Normal_Commit, Branch_Commit, Merge_Branch_Commit
 * or a Merge_Commit
 * @param curMaxRows The size of the nextCommits vector
 * @param prevMaxRow The size of the previous commits nextCommits vector
 * @return
 */
QVector<enum Commit::CommitType> *buildUpLane(QSet<int> branchedToRow, QSet<int> mergeFromRow,
                                              QVector<LibQGit2::QGitOId> &nextCommits, int activeRow,
                                              enum Commit::CommitType activeRowType, int curMaxRows, int prevMaxRow)
{
    QVector<enum Commit::CommitType> *currentRowState = new QVector<enum Commit::CommitType>;
    bool handledLastMerge = false;
    bool handledLastBranch = false;
    int handledMerges = 0;
    int handledBranches = 0;

    for (int i = 0; i < curMaxRows; i++)
    {
        bool containsBranch = branchedToRow.contains(i);
        bool containsMerge = mergeFromRow.contains(i);
        LibQGit2::QGitOId zero;
        bool containsZero = nextCommits[i] == zero;

        // anything before active row is an empty lane or non commit lane
        if (i < activeRow )
        {
            if (containsZero)
            {
                currentRowState->append(Commit::EMPTY_LANE);
            }
            else
            {
                currentRowState->append(Commit::NO_COMMIT);
            }
        }
        // we have worked out the lane type so no work needs to be done
        else if (i == activeRow)
        {
            currentRowState->append(activeRowType);
        }
        else if (activeRowType == Commit::BRANCH_COMMIT)
        {

            if (containsBranch)
            {
                handledLastBranch = appendRowTypeEqual (currentRowState, Commit::BRANCH_COMMIT_UP, handledBranches, branchedToRow.size());
            }
            else
            {
                handleRowTypeNotEqual (currentRowState, containsZero, handledLastBranch);
            }
        }
        else if (activeRowType == Commit::MERGE_COMMIT)
        {
            if (containsMerge)
            {
                handledLastMerge = appendRowTypeEqual (currentRowState, Commit::MERGE_COMMIT_DOWN, handledMerges, mergeFromRow.size() - 1);
            }
            else
            {
                handleRowTypeNotEqual (currentRowState, containsZero, handledLastMerge);
            }
        }
        else if (activeRowType == Commit::BRANCH_MERGE_COMMIT)
        {
            if (containsMerge && containsBranch)
            {
                handledMerges++;
                handledBranches++;

                if (handledMerges == mergeFromRow.size() -1)
                    handledLastMerge = true;

                if (handledBranches == branchedToRow.size())
                    handledLastBranch = true;

                if (handledLastBranch && handledLastMerge)
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_BOTH);
                else
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_BOTH_H);

            }
            else if (containsMerge)
            {
                handledLastMerge = branchMergeAppend (currentRowState, Commit::BRANCH_MERGE_COMMIT_DOWN, handledMerges,
                                   mergeFromRow.size() - 1, handledLastBranch);
            }
            else if (containsBranch)
            {
                handledLastBranch = branchMergeAppend (currentRowState, Commit::BRANCH_MERGE_COMMIT_UP, handledBranches,
                                   branchedToRow.size(), handledLastMerge);
            }
            else
            {
                bool handledBoth = handledLastBranch && handledLastMerge;
                handleRowTypeNotEqual (currentRowState, containsZero, handledBoth);

                if (!containsZero && i == curMaxRows -1 && i == branchedToRow.size() -1)
                {
                    currentRowState->append(Commit::BRANCH_MERGE_COMMIT_UP);
                }
            }
        }
        else if (activeRowType == Commit::NORMAL_COMMIT)
        {
            // add empty lanes or a single lane with no commit
            if (containsZero)
            {
                currentRowState->append(Commit::EMPTY_LANE);
            }
            else
            {
                currentRowState->append(Commit::NO_COMMIT);
            }
        }

    }

    branchsExtras (currentRowState, prevMaxRow, branchedToRow, activeRowType, handledBranches);

    return currentRowState;
}

/**
 * @brief acRepo::populateCommit This function works out what type of commit it is (eg a branch commit / a
 * merge commit / a branches merge commit or just a normal commit ). This information is then used
 * to create a Commit object.
 *
 * @param commit - The libgit2 commit object which on which we create the Commit out of
 * @param nextCommits - A Vector containing a list of the next oid to match against
 * @param prevCommit - The previous commit which is used to speed up creation of the lanes and
 * for the knowledge of the number of rows it contains.
 * @return A new Commit object.
 */
Commit* acRepo::populateCommit(LibQGit2::QGitCommit &commit, QVector<LibQGit2::QGitOId> &nextCommits, Commit *prevCommit)
{
    enum Commit::CommitType type = Commit::NO_COMMIT;
    bool mergeCommit = false;
    QSet<int> branchedToRow;
    QSet<int> mergeFromRow;
    int count = 0;
    int activeRow = -1;

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
                branchedToRow.insert(nextCommitIndex);
            }
            count++;
            //reset commit to zero
            LibQGit2::QGitOId zero;
            nextCommits[nextCommitIndex] = zero;
        }
    }

    // reduces size of nextCommits
    for (int removeIndex = nextCommits.size() - 1; removeIndex > activeRow + 1; removeIndex--)
    {
        LibQGit2::QGitOId zero;
        if (nextCommits[removeIndex] == zero)
        {
            nextCommits.remove(removeIndex);
        }
        else
        {
            // break early if index isn't a empty index.
            break;
        }
    }

    // Loops over parents of this commit and add to nextCommits List.
    for (uint i = 0; i < commit.parentCount() && commit.parentCount() > 1; i++)
    {
        bool added = false;
        for (int k = activeRow; k < nextCommits.size(); k++)
        {
            //default value of git oid is 0
            LibQGit2::QGitOId zero;
            if (nextCommits[k] == zero)
            {
                nextCommits[k] = commit.parent(i).oid();
                added = true;
                mergeFromRow.insert(k);
                break;
            }
        }
        if (added == false)
        {
            nextCommits.append(commit.parent(i).oid());
            mergeFromRow.insert(nextCommits.size() - 1);
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


    Commit::Lane lane;
    lane.activeRow = activeRow;
    if(type == Commit::NORMAL_COMMIT && prevCommit && prevCommit->getActiveRowNumber() == activeRow )
    {
        lane.lanes = prevCommit->getCurrentRowState();
    }
    else
    {
        int maxRows;
        if (!prevCommit)
        {
            maxRows = 1;
        }
        else
        {
            maxRows = prevCommit->getMaxRows();
        }
        lane.lanes = buildUpLane(branchedToRow, mergeFromRow, nextCommits, activeRow,type, nextCommits.size(), maxRows);
    }
    lane.type = type;

    Commit* newCommit = new Commit (commit, lane, nextCommits.size());

    return newCommit;
}
