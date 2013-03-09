#include "acrepo.h"
#include "commit.h"


#include <QDebug>

acRepo::acRepo(QString directory)
{
    repo.open(directory);

    // set walk commit to head
    LibQGit2::QGitCommit walkCommit = repo.lookupCommit(repo.head().oid());
    LibQGit2::QGitRevWalk walker(repo);

    QVector<LibQGit2::QGitOId> nextCommits;
    nextCommits.reserve(25);

    nextCommits.insert(0, repo.head().oid());
    Commit* newCommit = populateCommit(walkCommit, nextCommits, 1);
    commits.append (newCommit);
    walker.setSorting(LibQGit2::QGitRevWalk::Topological);

    walker.push(walkCommit);
    walker.next(walkCommit);

    while (walker.next(walkCommit) == true)
    {
        newCommit = populateCommit(walkCommit, nextCommits, newCommit->getMaxRow());
        commits.append(newCommit);
    }
//    Lanes = constructLanes();
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

Commit* acRepo::populateCommit(LibQGit2::QGitCommit &commit, QVector<LibQGit2::QGitOId> &nextCommits, int prevMaxRow)
{
    enum Commit::CommitType type = Commit::NORMAL_COMMIT;
    int count = 0;
    bool mergeCommit = false;

    Commit::Lane lane;

    lane.mergeFromRow = -1;
    lane.branchedToRow = -1;
    lane.activeRow = -1;
    lane.maxRows = prevMaxRow;
    lane.type = Commit::NO_COMMIT;




    // count the next commits that have the same oid as the commit
    for (int nextCommitIndex = 0; nextCommitIndex < nextCommits.size(); nextCommitIndex++)
    {
        LibQGit2::QGitOId nextCommit = nextCommits.at(nextCommitIndex);
        if (nextCommit == commit.oid())
        {
            // increament count of children with this commit
            if (count == 0)
            {
                lane.activeRow = nextCommitIndex;
            }
            else
            {
                lane.branchedToRow = nextCommitIndex;
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
        for (int k = lane.activeRow; k < nextCommits.size(); k++)
        {
            //default value of gitoid is 0
            LibQGit2::QGitOId zero;
            if (nextCommits[k] == zero)
            {
                nextCommits[k] = commit.parent(i).oid();
                added = true;
                lane.mergeFromRow = k;
                //prevMaxRow = k;
                break;
            }
        }
        if (added == false)
        {
            nextCommits.append(commit.parent(i).oid());
            lane.mergeFromRow = nextCommits.size() - 1;
            //prevMaxRow = nextCommits.size();
        }

        // more than one parent commit so must be a merge of some sort.
        mergeCommit = true;
    }

    if (mergeCommit && count == 1)
    {
        lane.type = Commit::MERGE_COMMIT;
        goto create;
    }

    // if merge commit and count > 1 then
    // we have a merge commit
    if (mergeCommit && count > 1)
    {
        lane.type = Commit::BRANCH_MERGE_COMMIT;

        goto create;
    }

    if (count > 1)
    {
        // remove current commits that match commit
        lane.type = Commit::BRANCH_COMMIT;

        //make sure this isn't the last commit
        if (commit.parentCount() != 0)
        {
            for (int k = lane.activeRow; k < nextCommits.size(); k++)
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

    if (count == 1)
    {
        lane.type = Commit::NORMAL_COMMIT;
        if (commit.parentCount() != 0)
        {
             nextCommits[lane.activeRow] = commit.parent(0).oid();
        }

        goto create;
    }


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

 create:
    //find last Row
    for (int i = nextCommits.size() - 1; i >= 0; i--)
    {
        LibQGit2::QGitOId zero;
        if (nextCommits[i] != zero)
        {
            prevMaxRow = i + 1;
            break;
        }
    }

    QVector<enum Commit::CommitType> *currentRowState = new QVector<enum Commit::CommitType>;
    bool handledLastMerge = false;
    bool handledLastBranch = false;
    for (int i = 0; i < prevMaxRow; i++)
    {
        // anything before active row is an empty lane or non commit lane
        if (i < lane.activeRow )
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
        else if (i == lane.activeRow)
        {
            currentRowState->append(lane.type);
            continue;
        }

        //TODO this does not handle multiple branches off the same commit
        // handle the case of a branch commit
        if (lane.type == Commit::BRANCH_COMMIT)
        {
            if (i == lane.branchedToRow)
            {
                currentRowState->append(Commit::BRANCH_COMMIT_UP);
                handledLastBranch = true;
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

        //TODO this does not handle multiple merges
        if (lane.type == Commit::MERGE_COMMIT)
        {
            // if last lane then type is merge down
            if (i == lane.mergeFromRow)
            {
                currentRowState->append(Commit::MERGE_COMMIT_DOWN);
                handledLastMerge = true;
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

        // TODO handle multiple of both types
        if (lane.type == Commit::BRANCH_MERGE_COMMIT)
        {
            if (i == lane.mergeFromRow && i == lane.branchedToRow)
            {
                currentRowState->append(Commit::BRANCH_MERGE_COMMIT_BOTH);
                handledLastMerge = true;
                handledLastBranch = true;

            }
            else if (i == lane.mergeFromRow)
            {
                currentRowState->append(Commit::BRANCH_MERGE_COMMIT_DOWN);
                handledLastMerge = true;

            }
            else if (i == lane.branchedToRow)
            {
                currentRowState->append(Commit::BRANCH_MERGE_COMMIT_UP);
                handledLastBranch = true;
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
                }
            }
            continue;
        }

    }

    lane.currentRowState = currentRowState;
    lane.currentState = nextCommits;
    lane.maxRows = prevMaxRow;
    qDebug() << "lane info row " << lane.activeRow << " maxRows " << lane.maxRows;
    Commit* newCommit = new Commit (commit, lane);

    return newCommit;
}
