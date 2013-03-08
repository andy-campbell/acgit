#include "difftextedit.h"

#include <QStringList>

diffTextEdit::diffTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void diffTextEdit::append(QString text)
{
    QColor color;

    // split on newlines
    QStringList strings = text.split("\n");

    foreach (QString line, strings)
    {
        if (line.startsWith('+'))
        {
            //TODO colour is too green need to dull it abit
            color = Qt::green;
        }
        else if ( line.startsWith('-'))
        {
            //TODO colour is too red need to dull it abit
            color = Qt::red;
        }
        else
        {
            color = Qt::white;
        }


        QTextCursor cursor(this->document());
        QTextBlockFormat bf = cursor.blockFormat();
        bf.setBackground(QBrush(color));
        cursor.movePosition(QTextCursor::End);
        cursor.insertBlock(bf);
        QTextCharFormat format;
        cursor.insertText(line, format);

        this->textCursor().movePosition(QTextCursor::End);
        this->ensureCursorVisible ();
    }
}
