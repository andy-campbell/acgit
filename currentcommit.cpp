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

#include <QTextStream>
#include <QFile>

#include "currentcommit.h"

currentCommit::currentCommit(const acRepo *repo, const Commit *from, const Commit *to)
    : _selectedCommit(to)
{
    diff = new LibQGit2::QGitDiff (repo->getRepo());

    if (to->getCommitType() == Commit::NO_COMMIT_WORKING_DIR)
    {
        diff->diffWorkingDir();
    }
    else
    {
        diff->diffCommits(from->getCommit(), to->getCommit());
    }
}

currentCommit::~currentCommit()
{
    delete diff;
}

QStringList currentCommit::getFileList()
{
    return diff->getFileChangedList();
}

QString currentCommit::getDetaForFile(QString &filename)
{
    QString delta = "";
    if (diff->getFileChangedList().contains(filename) == true)
    {
        delta = diff->getDeltasForFile(filename);
    }
    return delta;
}

const Commit *currentCommit::getCurrentSelectedCommit() const
{
    return _selectedCommit;
}

void currentCommit::savePatch(const QString path)
{
    QFile file(path);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    out << diff->print();
    file.close();
}

QString currentCommit::getPatchStats()
{
    return diff->diffStats();
}
