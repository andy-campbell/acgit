/**
 * This file is part of AcGit.
 *
 * AcGit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AcGit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */


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
            color = QColor(169, 245, 96);
        }
        else if ( line.startsWith('-'))
        {
            //TODO colour is too red need to dull it abit
            color = QColor(245, 153, 154);
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
