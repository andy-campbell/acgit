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

#include "acrepo.h"
#include "commit.h"

class revViewDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit revViewDelegate(acRepo *repo, QObject *parent = 0);

    virtual void paint(QPainter* p, const QStyleOptionViewItem& o, const QModelIndex &i) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& o, const QModelIndex &i) const;

    
signals:
    
public slots:
    
private:
    void paintGraph(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &i) const;
    void paintGraphLane(QPainter *p, int type, int x1, int x2, const QColor &col, const QColor &activeCol, const QBrush &back, bool active, bool last, bool branched) const;

    acRepo *_repo;
};

#endif // REVVIEWDELEGATE_H
