#ifndef CLONEPROGRESS_H
#define CLONEPROGRESS_H

#include <QDialog>

namespace Ui {
class CloneProgress;
}

class CloneProgress : public QDialog
{
    Q_OBJECT
    
public:
    explicit CloneProgress(QWidget *parent = 0);
    ~CloneProgress();

    void setIndexProgress(int percentage);
    void setNetworkProgress(int percentage);
    void setTotalProgress(int percentage);
signals:
    void cancel();

private slots:
    void on_buttonBox_rejected();

private:
    Ui::CloneProgress *ui;
};

#endif // CLONEPROGRESS_H
