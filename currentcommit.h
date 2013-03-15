#ifndef CURRENTCOMMIT_H
#define CURRENTCOMMIT_H

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


#include "../libqgit2/qgit2.h"

#include <QStringList>

#include "commit.h"
#include "acrepo.h"

class currentCommit
{
public:
    currentCommit(const acRepo *repo, const Commit *from, const Commit *to);

    virtual ~currentCommit();

    /**
     * @brief getFileList gets a list of files changed between
     * the from commit to the to commit.
     * @return The list of files
     */
    QStringList getFileList();

    /**
     * @brief getDetaForFile this function check if the filename
     * is in the filelist. If it is not then it returns an empty string
     * otherwise it will get the changes for the file between the
     * two commits.
     * @param filename The file to get the delta for
     * @return  The delta of the file
     */
    QString getDetaForFile(QString &filename);

    /**
     * @brief getCurrentSelectedCommit this function returns the "to" commit
     * @return return the "to" commit which is the current selected commit
     */
    const Commit *getCurrentSelectedCommit();

private:
    const Commit *_selectedCommit;
    LibQGit2::QGitDiff *diff;
};

#endif // CURRENTCOMMIT_H
