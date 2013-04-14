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

class Commit;
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

    QStringList getTags();

    QStringList getBranches();

    /**
     * @brief lookupTag This function does a lookup of a tag which it
     * then uses the oid to lookup the commit and returns the index of
     * it in the commits vector.
     * @param tagName The name of the tag to lookup
     *
     * @return
     */
    int lookupTag (QString tagName);

    int lookupBranch(QString branchName);
signals:
    void repoOpened();

private:
    Commit *populateCommit(LibQGit2::QGitCommit &commit, QVector<LibQGit2::QGitOId> &nextCommits, Commit *prevCommit);
    LibQGit2::QGitRepository repo;
    QVector<Commit*> commits;
    // key is oid and value is item in commits. Another potential way todo this is implement a multi key map
    QMap<QByteArray, int> commitLookup;
    // key is the tags name and the value is the oid of the tag so it can be used for lookup
    QMap<QString, QByteArray> tagList;
};

#endif // ACREPO_H
