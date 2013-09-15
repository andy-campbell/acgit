#include <QFileDialog>

#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

Options::~Options()
{
    delete ui;
}

void Options::on_browse_clicked()
{
    QString executableLocation = QFileDialog::getOpenFileName(this, tr("Diff tool executable"),
                                                     QDir::home().path());

    if(executableLocation.isEmpty())
        return;

    ui->diffToolLineEdit->setText(executableLocation);
}

void Options::on_buttonBox_accepted()
{
    if(!ui->diffToolLineEdit->text().isEmpty())
        emit updatedExternalDiffTool(ui->diffToolLineEdit->text());

    close();
}

void Options::on_buttonBox_rejected()
{
    close();
}
