#include "cloneprogress.h"
#include "ui_cloneprogress.h"

CloneProgress::CloneProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CloneProgress)
{
    ui->setupUi(this);
    ui->indexProgress->setValue(0);
    ui->networkProgress->setValue(0);
    ui->totalProgress->setValue(0);
}

CloneProgress::~CloneProgress()
{
    delete ui;
}

void CloneProgress::on_buttonBox_rejected()
{
    emit cancel();
}

void CloneProgress::setIndexProgress(int percentage)
{
    if (percentage >= 0 && percentage <= 100)
    {
        ui->indexProgress->setValue(percentage);
    }
}

void CloneProgress::setNetworkProgress(int percentage)
{
    if (percentage >= 0 && percentage <= 100)
    {
        ui->networkProgress->setValue(percentage);
    }
}

void CloneProgress::setTotalProgress(int percentage)
{
    if (percentage >= 0 && percentage <= 100)
    {
        ui->totalProgress->setValue(percentage);
    }
}
