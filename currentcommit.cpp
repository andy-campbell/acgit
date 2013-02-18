#include "currentcommit.h"

currentCommit::currentCommit(const acRepo *repo, const Commit *from, const Commit *to)
    : _selectedCommit(to)
{
    diff = new LibQGit2::QGitDiff (repo->getRepo(), from->getCommit(), to->getCommit());
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

const Commit *currentCommit::getCurrentSelectedCommit()
{
    return _selectedCommit;
}
