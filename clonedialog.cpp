#include <QFileDialog>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QBuffer>

#include "clonedialog.h"
#include "ui_clonedialog.h"

CloneDialog::CloneDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CloneDialog)
{
    ui->setupUi(this);
}

CloneDialog::~CloneDialog()
{
    delete ui;
}

void CloneDialog::on_saveToBrowse_clicked()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::home().path());

    ui->saveToLineEdit->setText(folderName);
}


void CloneDialog::on_cloneButton_clicked()
{
    if(ui->saveToLineEdit->text().length() == 0)
    {
        // throw error
    }

    if(ui->urlLineEdit->text().length() == 0)
    {
        // throw error
    }

    startClone();
}

void CloneDialog::handleCloneDialogUpdate(AcGit::Clone *clone)
{
    progress->setIndexProgress(clone->indexPercentage());
    progress->setTotalProgress(clone->checkoutPercentage());
    progress->update();
}

void CloneDialog::updateFetchProgress(AcGit::Clone *clone)
{
    progress->setNetworkProgress(clone->indexPercentage());
    progress->setTotalProgress(clone->checkoutPercentage());
    progress->update();
}

AcGit::Repository* CloneDialog::getClonedRepo()
{
    return repo;
}

AcGit::Credintials CloneDialog::handleCredintialRequest()
{
    AcGit::Credintials creds("dummy", "dummy");
    return creds;
}

void CloneDialog::displayError(QString error)
{
    QMessageBox::warning(this, tr("Error has occured"), error,
                        QMessageBox::Ok|QMessageBox::Default, QMessageBox::Escape);
    progress->hide();
}

void CloneDialog::fetchProgress(int indexPercentage, int networkPercentage)
{
    progress->setIndexProgress(indexPercentage);
    progress->setNetworkProgress(networkPercentage);
}

void CloneDialog::checkoutProgress(int checkoutPercentage)
{
    progress->setTotalProgress(checkoutPercentage);
}

void CloneDialog::getCredentials()
{
    // pop up question box
    AcGit::Credintials creds("test", "test");
    // write info into shared mem;
    saveCredentials("username", creds.getUsername());
    saveCredentials("password", creds.getPassword());

}

void CloneDialog::saveCredentials(QString sharedName, QString data)
{
    QSharedMemory credentialsShared(sharedName, this);
    // load into shared memory

    credentialsShared.attach();
    credentialsShared.lock();
    QString *to = (QString*)credentialsShared.data();
    QString *from = new QString(data);
    memcpy(to, from, sizeof(*from));
    credentialsShared.unlock();
}

void CloneDialog::clonedRepo(AcGit::Repository *repo)
{
    progress->close();
    delete progress;
    progress = nullptr;
    this->repo = repo;
    emit cloneCompleted();
}

void CloneDialog::startClone()
{
    QString url = ui->urlLineEdit->text();
    QString path = ui->saveToLineEdit->text();

    progress = new CloneProgress(this);

    QThread *thread = new QThread();
    CloneRepo *clone = new CloneRepo(url ,path);
    clone->moveToThread(thread);
    connect(clone, SIGNAL(error(QString)), this, SLOT(displayError(QString)));
    connect(thread, SIGNAL(started()), clone, SLOT(process()));
    connect(clone, SIGNAL(finished()), thread, SLOT(quit()));
    connect(clone, SIGNAL(finished()), clone, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(clone, SIGNAL(fetchIndexProgress(int,int)), this, SLOT(fetchProgress(int,int)));
    connect(clone, SIGNAL(checkoutProgress(int)), this, SLOT(checkoutProgress(int)));
    connect(clone, SIGNAL(credentialsRequest()), this, SLOT(getCredentials()));
    connect(clone, SIGNAL(newRepo(AcGit::Repository*)), this, SLOT(clonedRepo(AcGit::Repository*)));
    connect(progress, SIGNAL(cancel()), clone, SLOT(cancelClone()));
    thread->start();

    this->hide();
    progress->show();
}


CloneRepo::CloneRepo(QString url, QString path)
{
    this->url = url;
    this->path = path;
    cancel = false;
}

void CloneRepo::loadCredentials()
{



}


CloneRepo::~CloneRepo()
{

}

void CloneRepo::updateCheckoutProgress(AcGit::Clone *clone)
{
    emit checkoutProgress(clone->checkoutPercentage());
}

bool CloneRepo::handleCloneDialogUpdate(AcGit::Clone *clone)
{
    emit fetchIndexProgress(clone->indexPercentage(), clone->networkPercentage());
    return cancel;
}

AcGit::Credintials CloneRepo::handleCredintialRequest()
{
    QSharedMemory credsPassword("password");
    credsPassword.create(1024);
    credsPassword.attach();

    QSharedMemory credsUsername("username");
    credsUsername.create(1024);
    credsUsername.attach();

    emit credentialsRequest();

    //Read creds
    QString username;
    QString password;

    credsPassword.lock();
    password = QByteArray((char*)credsPassword.constData(), credsPassword.size());
    credsPassword.unlock();

    credsUsername.lock();
    username = QByteArray((char*)credsUsername.constData(), credsUsername.size());
    credsUsername.unlock();

    credsPassword.detach();
    credsUsername.detach();

    return AcGit::Credintials(username, password);
}

void CloneRepo::process()
{
    try
    {
        cloneRepo = new  AcGit::Clone(url, path, [this] (AcGit::Clone *clone) { updateCheckoutProgress(clone); },
                        [this] (AcGit::Clone *clone) -> bool { return handleCloneDialogUpdate(clone); },
                        [this] () -> AcGit::Credintials { return handleCredintialRequest(); });
        emit newRepo(cloneRepo->getRepo());
    }
    catch (AcGit::GitException e)
    {
        emit error(e.exceptionMessage());
    }
    emit finished();
}

void CloneRepo::cancelClone()
{
    cancel = true;
}
