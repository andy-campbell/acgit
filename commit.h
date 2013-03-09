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
        //TODO add these for more info
        //QVector<LibQGit2::QGitOId> parents;
        //QVector<LibQGit2::QGitOId> children;
        enum CommitType type;
        int branchedToRow;
        int mergeFromRow;
        int maxRows;
        QVector<LibQGit2::QGitOId> currentState;
        QVector<enum CommitType> *currentRowState;

    };

    Commit(LibQGit2::QGitCommit commit, struct Lane laneinfo);

    virtual ~Commit();

    int getNumParents();

    void setNumParents(int parentsCount = 1);

    int getRow();

    LibQGit2::QGitCommit getCommit() const;

    enum CommitType getCommitType();

    int getMaxRow();

    int getBranchRow();

    int getMergedFromRow();

    QVector<LibQGit2::QGitOId> getState();

    QVector<enum CommitType>* getCurrentRowState();

private:
    LibQGit2::QGitCommit _commit;
    enum CommitType _type;
    int _numParents;
    struct Lane _laneInfo;


};

#endif // COMMIT_H
