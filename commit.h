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
#include "acrepo.h"

#include <QVector>

class acRepo;

class Commit
{
public:
    enum CommitType
    {
        NO_COMMIT = 0,
        NO_COMMIT_H,
        NO_COMMIT_WORKING_DIR,
        EMPTY_LANE,
        EMPTY_LANE_H,
        NORMAL_COMMIT,
        NORMAL_COMMIT_H,
        BRANCH_COMMIT,
        BRANCH_COMMIT_UP,
        BRANCH_COMMIT_UP_H,
        MERGE_COMMIT,
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

    int getActiveRowNumber();

    LibQGit2::QGitCommit getCommit() const;

    enum CommitType getCommitType() const;

    QVector<enum CommitType>* getCurrentRowState();

    int getMaxRows();

    /**
     * @brief addTag add a tag to the current commit. This is to be used only
     * if the tag already exists. If it does not exist in the git repository
     * then call createTag
     * @param tag The name of the tag to add.
     */
    void addTag(QString tagName);

    /**
     * @brief createTag adds a tag to the current commit. This adds the tag
     * to this commit in the git repository .
     * @param tagName The name of the tag to add.
     */
    void createTag (acRepo *repo, QString tagName);

    /**
     * @brief removeTag This call removes the tag with tagName from this commit.
     * Warning: Don't Call this if trying to remove tag from only this Object as it
     * will remove from the git repository.
     * @param tagName
     */
    void removeTag(acRepo *repo, QString tagName);

    /**
     * @brief createBranch Creates a git branch and adds it to this objects branch list.
     * @param repo Repo that contains the commit.
     * @param branchName Name of the new branch to create.
     */
    void createBranch (acRepo *repo, QString branchName);

    /**
     * @brief removeBranch deletes a branch from git
     * @param repo Repo that contains the branch
     * @param name Name of the branch to remove.
     */
    void removeBranch (acRepo *repo, QString name);

    /**
     * @brief addBranch adds an existing branch to the current commit. This is
     * to be used only if the branch already exists. If the branch does not exist
     * use createBranch.
     * @param branchName Name of the branch to add.
     */
    void addBranch(QString branchName);

    /**
     * @brief getBranches This returns the branches which are associated with this commit.
     * If the commit has no branches then the commit QStringList will be empty.
     * @return A QStringList containing the list of branches for this commit.
     */
    QStringList getBranches();

    /**
     * @brief getTags This returns the tags which are associated with this commit. If the
     * commit has no tags then an empty QStringList will returned.
     * @return A QStringList containg the list of tags for this commit.
     */
    QStringList getTags();

    bool isWorkingDirectory();
private:
    LibQGit2::QGitCommit _commit;
    int _numParents;
    struct Lane _laneInfo;
    int _prevMaxRows;
    QStringList tags;
    QStringList branches;


};

#endif // COMMIT_H
