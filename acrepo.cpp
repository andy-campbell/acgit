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
    int activeLane = 0;
    int branchFromLane = -1;

    // count the next commits that have the same oid as the commit
    for (int nextCommitIndex = 0; nextCommitIndex < nextCommits.size(); nextCommitIndex++)
    {
        LibQGit2::QGitOId nextCommit = nextCommits.at(nextCommitIndex);
        if (nextCommit == commit.oid())
        {
            // increament count of children with this commit
            if (count == 0)
            {
                activeLane = nextCommitIndex;
            }
            else
            {
                branchFromLane = nextCommitIndex;
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
        for (int k = activeLane; k < nextCommits.size(); k++)
        {
            //default value of gitoid is 0
            LibQGit2::QGitOId zero;
            if (nextCommits[k] == zero)
            {
                nextCommits[k] = commit.parent(i).oid();
                added = true;
                prevMaxRow = k;
                break;
            }
        }
        if (added == false)
        {
            nextCommits.append(commit.parent(i).oid());
            prevMaxRow = nextCommits.size();
        }

        // more than one parent commit so must be a merge of some sort.
        mergeCommit = true;
    }

    if (mergeCommit && count == 1)
    {
        type = Commit::MERGE_COMMIT;
        goto create;
    }

    // if merge commit and count > 1 then
    // we have a merge commit
    if (mergeCommit && count > 1)
    {
        type = Commit::BRANCH_MERGE_COMMIT;

        goto create;
    }

    if (count > 1)
    {
        // remove current commits that match commit
        type = Commit::BRANCH_COMMIT;

        //make sure this isn't the last commit
        if (commit.parentCount() != 0)
        {
            for (int k = activeLane; k < nextCommits.size(); k++)
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
        type = Commit::NORMAL_COMMIT;
        if (commit.parentCount() != 0)
        {
             nextCommits[activeLane] = commit.parent(0).oid();
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

    Commit* newCommit = new Commit (commit, type, activeLane,branchFromLane, prevMaxRow);

    return newCommit;
}
