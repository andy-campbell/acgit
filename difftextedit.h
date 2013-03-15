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
