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

#include "currentdiff.h"

#include <QDebug>


/**
 *  Call for diff of all commits except working directory
 */
currentDiff::currentDiff(AcGit::Commit *from, AcGit::Commit *selectedCommit)
{
    this->selectedCommit = selectedCommit;
    this->diffFromCommit = from;

    if (from == nullptr)
    {
        diff = new AcGit::TreeDiff(nullptr, selectedCommit->tree());
    }
    else
    {
        diff = new AcGit::TreeDiff(from->tree(), selectedCommit->tree());
    }
    type = COMMIT;
}

/**
 * Call for diff of working directory
 */
currentDiff::currentDiff(AcGit::Commit *headCommit, bool isWorkingDirectory)
{
    if(isWorkingDirectory)
    {
        diff = new AcGit::WorkingDirDiff(headCommit->tree());
        type = WORKINGDIR;
    }
    else
    {
        diff = new AcGit::StagingDirDiff(headCommit->tree());
        type = STAGINGDIR;
    }
    this->selectedCommit = headCommit;
}


currentDiff::~currentDiff()
{
    delete diff;
}

QStringList currentDiff::getFileList()
{
    return diff->fileList();
}

QString currentDiff::getDetaForFile(QString &filename)
{
    QString delta;
    try
    {
        delta = diff->fileDiff(filename);
    }
    catch (AcGit::fileNotInDiffException e)
    {
        qCritical () << e.exceptionMessage();
    }

    return delta;
}

AcGit::Commit *currentDiff::getCurrentSelectedCommit()
{
    return selectedCommit;
}

AcGit::Commit *currentDiff::getCommitDiffedFrom()
{
    return diffFromCommit;
}

void currentDiff::savePatch(const QString path)
{
    QFile file(path);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    out << diff->printDiff();
    file.close();
}

QString currentDiff::getPatchStats()
{
    return diff->diffStats();
}

RowType currentDiff::getType()
{
    return type;
}
