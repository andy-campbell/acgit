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

    nextCommits.append(walkCommit.oid());
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

Commit* acRepo::populateCommit(LibQGit2::QGitCommit &commit, QList<LibQGit2::QGitOId> &nextCommits, int prevMaxRow)
{
    enum Commit::CommitType type = Commit::NORMAL_COMMIT;
    int count = 0;
    bool mergeCommit = false;

    // count the next commits that have the same oid as the commit
    for (int nextCommitIndex = 0; nextCommitIndex < nextCommits.size(); nextCommitIndex++)
    {
        LibQGit2::QGitOId nextCommit = nextCommits.at(nextCommitIndex);
        if (nextCommit == commit.oid())
        {
            // increament count of children with this commit
            count++;
        }
    }

    // Loops over parents of this commit and add to nextCommits List.
    for (uint i = 0; i < commit.parentCount() && commit.parentCount() > 1; i++)
    {
        nextCommits.append(commit.parent(i).oid());
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
            nextCommits.append(commit.parent(0).oid());
    }

    if (count == 1)
    {
        type = Commit::NORMAL_COMMIT;
        if (commit.parentCount() != 0)
        {
            for (int i = 0; i < nextCommits.size(); i++ )
            {
                if (nextCommits[i] == 0)
                {
                    nextCommits = commit.parent(0).oid();
                }
            }
            nextCommits.append(commit.parent(0).oid());
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
    if (type == Commit::BRANCH_COMMIT)
    {
        prevMaxRow--;
    }
    else if (type == Commit::MERGE_COMMIT)
    {
        prevMaxRow++;
    }
    Commit* newCommit = new Commit (commit, type, nextCommits.indexOf(commit.oid()),nextCommits.lastIndexOf(commit.oid()), prevMaxRow);

    // zero old commits
    for (int i = 0; i < nextCommits.size(); i++)
    {
        if (nextCommits[i] == commit.oid())
        {
            nextCommits[i] = 0;
        }
    }

    return newCommit;
}
