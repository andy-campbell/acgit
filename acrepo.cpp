#include "acrepo.h"

acRepo::acRepo(QString directory)
{
    repo.open(directory);

    emit repoOpened ();
}

acRepo::~acRepo()
{
}

LibQGit2::QGitRepository acRepo::getRepo()
{
    return repo;
}

void acRepo::setRepo(const LibQGit2::QGitRepository & value)
{
    repo = value;
}
