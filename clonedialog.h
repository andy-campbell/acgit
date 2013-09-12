#ifndef CLONEDIALOG_H
#define CLONEDIALOG_H

#include <QDialog>
#include <QString>
#include <QSharedMemory>

#include "AcGitGlobal.h"
#include "Credintials.h"
#include "Clone.h"

#include "cloneprogress.h"

namespace Ui {
class CloneDialog;
}

class CloneRepo : public QObject
{
    Q_OBJECT

public:
    CloneRepo(QString url, QString path);
    ~CloneRepo();

    bool handleCloneDialogUpdate(AcGit::Clone *clone);
    AcGit::Credintials handleCredintialRequest();

    void updateCheckoutProgress(AcGit::Clone *clone);
public slots:
    void process();
    void cancelClone();

signals:
    void finished();
    void error(QString error);
    void fetchIndexProgress(int indexPercentage, int networkPercentage);
    void checkoutProgress(int checkoutPercentage);
    void newRepo(AcGit::Repository *repo);
    void credentialsRequest();
private:
    AcGit::Clone *cloneRepo;
    QString url;
    QString path;
    bool cancel;

    void loadCredentials();
};

class CloneDialog : public QDialog
{
    Q_OBJECT
public:
    CloneDialog(QWidget *parent = 0);
    virtual ~CloneDialog();
    void handleCloneDialogUpdate(AcGit::Clone *clone);
    AcGit::Credintials handleCredintialRequest();

    void updateFetchProgress(AcGit::Clone *clone);

    AcGit::Repository *getClonedRepo();
signals:
    void cloneCompleted();
    void cancelClone();

private slots:
    void on_saveToBrowse_clicked();

    void on_cloneButton_clicked();

    void displayError(QString error);
    void fetchProgress(int indexPercentage, int networkPercentage);
    void checkoutProgress(int checkoutPercentage);
    void clonedRepo(AcGit::Repository *repo);
    void getCredentials();
    void on_pushButton_clicked();

private:
    Ui::CloneDialog *ui;
    void startClone();
    AcGit::Repository *repo;
    CloneProgress *progress;


    void saveCredentials(QString sharedName, QString data);
    void populateSignalSlots(QThread *thread, CloneRepo *clone);
    void showProgress();
};



#endif // CLONEDIALOG_H
