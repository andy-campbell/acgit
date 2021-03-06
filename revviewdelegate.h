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


#ifndef REVVIEWDELEGATE_H
#define REVVIEWDELEGATE_H

#include <QItemDelegate>
#include <QTreeView>
#include <QItemDelegate>
#include <QSortFilterProxyModel>
#include <QRegExp>
#include <QPen>
#include <QPainter>
#include <QModelIndex>

#include "AcGitGlobal.h"
#include "mainwindowrevview.h"
#include "types.h"

class MainWindowRevView;

class revViewDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit revViewDelegate(AcGit::Repository *repo, MainWindowRevView *revView, QObject *parent = 0);

    virtual void paint(QPainter* p, const QStyleOptionViewItem &o, const QModelIndex &i) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& o, const QModelIndex &i) const;

    
signals:
    
public slots:
    
private:

    void addTextPixmap(QPixmap **pp, QString data, const QStyleOptionViewItem &opt) const;
    QPixmap *getRefsPixmap(AcGit::Commit *commit, QStyleOptionViewItem &opt) const;
    void paintShort(QPainter *p, QStyleOptionViewItem opt, const QModelIndex &index) const;
    void paintGraph(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &index) const;
    void paintGraphLane(QPainter *p, int type, int x1, int x2, const QColor &col, const QColor &activeCol, const QBrush &back, bool firstCommit) const;
    void paintNonCommitGraph(QPainter *p, const QStyleOptionViewItem &opt, RowType type) const;

    AcGit::Repository *repo;
    MainWindowRevView *revView;
    bool hasWorkingDirectoryChanges() const;
    bool hasStagingDirectoryChanges() const;
    int findCommitIndex(int modelIndex) const;
    RowType getRowType(int index) const;
};

#endif // REVVIEWDELEGATE_H
