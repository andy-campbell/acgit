#ifndef DIFFTEXTEDIT_H
#define DIFFTEXTEDIT_H

#include <QTextEdit>

class diffTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit diffTextEdit(QWidget *parent = 0);
    
    void append(QString text);
signals:
    
public slots:
    
};

#endif // DIFFTEXTEDIT_H
