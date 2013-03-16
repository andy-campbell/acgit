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

#ifndef ACREPO_H
#define ACREPO_H

#include <QString>
#include <QObject>
#include <QVector>

#include "../libqgit2/qgit2.h"

#include "commit.h"

/**
 * This class handles the calls about the repo
 */
class acRepo : public QObject
{
    Q_OBJECT

public:
    explicit acRepo(QString directory);

    virtual ~acRepo();

    LibQGit2::QGitRepository getRepo() const;

    void setRepo(const LibQGit2::QGitRepository &value);

    void addCommit (const LibQGit2::QGitCommit &commit);

    QVector<Commit*> getAllCommits();

signals:
    void repoOpened();

private:
    Commit *populateCommit(LibQGit2::QGitCommit &commit, QVector<LibQGit2::QGitOId> &nextCommits, int prevMaxRow);
    LibQGit2::QGitRepository repo;
    QVector<Commit*> commits;
};

#endif // ACREPO_H
