#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "acrepo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void revWalk();
    void findAllBranches();
private slots:
    void on_actionOpen_triggered();
    void loadRepo();

    void on_revListTable_cellClicked(int row, int column);

private:
    Ui::MainWindow *ui;

    // TODO remove from here
    acRepo *repo;
};

#endif // MAINWINDOW_H
