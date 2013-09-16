#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>

#include "mainwindow.h"

namespace Ui {
class Options;
}

class MainWindow;

class Options : public QDialog
{
    Q_OBJECT
    
public:
    explicit Options(MainWindow* mainWindow, QWidget *parent = 0);
    ~Options();

signals:
    void updatedExternalDiffTool(QString toolLocation);
    
private slots:
    void on_browse_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::Options *ui;
    MainWindow *mainWindow;
};

#endif // OPTIONS_H
