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

#include "revviewdelegate.h"

#include <QDebug>
revViewDelegate::revViewDelegate( acRepo* repo,
QObject *parent) :
    QItemDelegate(parent)
{
    _repo = repo;
}


QSize revViewDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
{
    // todo make this less const
    return QSize(80, 26);
}

static QColor blend(const QColor& col1, const QColor& col2, int amount = 128) {

    // Returns ((256 - amount)*col1 + amount*col2) / 256;
    return QColor(((256 - amount)*col1.red()   + amount*col2.red()  ) / 256,
                  ((256 - amount)*col1.green() + amount*col2.green()) / 256,
                  ((256 - amount)*col1.blue()  + amount*col2.blue() ) / 256);
}

// colors
const QColor BROWN        = QColor(150, 75, 0);
const QColor ORANGE       = QColor(255, 160, 50);
const QColor DARK_ORANGE  = QColor(216, 144, 0);
const QColor LIGHT_ORANGE = QColor(255, 221, 170);
const QColor LIGHT_BLUE   = QColor(85, 255, 255);
const QColor PURPLE       = QColor(221, 221, 255);
const QColor DARK_GREEN   = QColor(0, 205, 0);

void revViewDelegate::paintGraphLane(QPainter* p, int type, int x1, int x2,
                                      const QColor& col, const QColor& activeCol, const QBrush& back) const
{

    int h = 26 / 2;
    int m = (x1 + x2) / 2;
    int r = (x2 - x1) / 3;
    int d =  2 * r;

    #define P_CENTER m , h
    #define P_0      x2, h      // >
    #define P_90     m , 0      // ^
    #define P_180    x1, h      // <
    #define P_270    m , 2 * h  // v
    #define DELTA_UR 2*(x1 - m), 2*h ,   0*16, 90*16  // -,
    #define DELTA_DR 2*(x1 - m), 2*-h, 270*16, 90*16  // -'
    #define DELTA_UL 2*(x2 - m), 2*h ,  90*16, 90*16  //  ,-
    #define DELTA_DL 2*(x2 - m), 2*-h, 180*16, 90*16  //  '-
    #define CENTER_UR x1, 2*h, 225
    #define CENTER_DR x1, 0  , 135
    #define CENTER_UL x2, 2*h, 315
    #define CENTER_DL x2, 0  ,  45
    #define R_CENTER m - r, h - r, d, d

    static QPen myPen(Qt::black, 2); // fast path here

    // arc
    switch (type) {
    case Commit::MERGE_COMMIT_DOWN:
    case Commit::MERGE_COMMIT_DOWN_H:
    case Commit::BRANCH_MERGE_COMMIT_DOWN:
    case Commit::BRANCH_MERGE_COMMIT_DOWN_H:
    {
        // arc down
        QConicalGradient gradient(CENTER_DR);
        gradient.setColorAt(0.375, activeCol);
        gradient.setColorAt(0.625, col);
        myPen.setBrush(gradient);
        p->setPen(myPen);
        p->drawArc(P_CENTER, DELTA_UR);
        break;
    }
    case Commit::BRANCH_COMMIT_UP:
    case Commit::BRANCH_COMMIT_UP_H:
    case Commit::BRANCH_MERGE_COMMIT_UP:
    case Commit::BRANCH_MERGE_COMMIT_UP_H:
    {
        // arc up
        QConicalGradient gradient(CENTER_UL);
        gradient.setColorAt(0.375, activeCol);
        gradient.setColorAt(0.625, col);
        myPen.setBrush(gradient);
        p->setPen(myPen);
        p->drawArc(P_CENTER, DELTA_DR);
        break;
    }
    case Commit::BRANCH_MERGE_COMMIT_BOTH:
    case Commit::BRANCH_MERGE_COMMIT_BOTH_H:
    {
        // arc up
        QConicalGradient gradientUp(CENTER_UL);
        gradientUp.setColorAt(0.375, activeCol);
        gradientUp.setColorAt(0.625, col);
        myPen.setBrush(gradientUp);
        p->setPen(myPen);
        p->drawArc(P_CENTER, DELTA_DR);

        // arc down
        QConicalGradient gradientDown(CENTER_DR);
        gradientDown.setColorAt(0.375, activeCol);
        gradientDown.setColorAt(0.625, col);
        myPen.setBrush(gradientDown);
        p->setPen(myPen);
        p->drawArc(P_CENTER, DELTA_UR);
        break;
    }
    default:
        break;
    }

    myPen.setColor(col);
    p->setPen(myPen);

    // vertical line
    switch (type) {
    case Commit::NORMAL_COMMIT:
    case Commit::BRANCH_COMMIT:
    case Commit::MERGE_COMMIT:
    case Commit::BRANCH_MERGE_COMMIT:
    case Commit::NO_COMMIT:
    case Commit::NO_COMMIT_H:
        p->drawLine(P_90, P_270);
        break;
    default:
        break;
    }

    myPen.setColor(activeCol);
    p->setPen(myPen);

    // horizontal line
    switch (type) {
    case Commit::MERGE_COMMIT_H:
    case Commit::MERGE_COMMIT_DOWN_H:
    case Commit::BRANCH_MERGE_COMMIT_BOTH_H:
    case Commit::BRANCH_MERGE_COMMIT_DOWN_H:
    case Commit::BRANCH_MERGE_COMMIT_UP_H:
    case Commit::BRANCH_COMMIT_H:
    case Commit::BRANCH_COMMIT_UP_H:
    case Commit::NO_COMMIT_H:
    case Commit::EMPTY_LANE_H:
        p->drawLine(P_180, P_0);
        //p->drawLine(P_CENTER, P_0);
        break;
    case Commit::MERGE_COMMIT:
    case Commit::BRANCH_COMMIT:
    case Commit::BRANCH_MERGE_COMMIT:
        p->drawLine(P_CENTER, P_0);
    default:
        break;
    }

    // center symbol, e.g. rect or ellipse
    switch (type) {
    case Commit::MERGE_COMMIT:
    case Commit::BRANCH_COMMIT:
    case Commit::BRANCH_MERGE_COMMIT:
        p->setPen(Qt::NoPen);
        p->setBrush(col);
        p->drawRect(R_CENTER);
        break;
    case Commit::NORMAL_COMMIT:
        p->setBrush(back);
        p->drawEllipse(R_CENTER);
        break;
    default:
        break;
    }
    #undef P_CENTER
    #undef P_0
    #undef P_90
    #undef P_180
    #undef P_270
    #undef DELTA_UR
    #undef DELTA_DR
    #undef DELTA_UL
    #undef DELTA_DL
    #undef CENTER_UR
    #undef CENTER_DR
    #undef CENTER_UL
    #undef CENTER_DL
    #undef R_CENTER
}

void revViewDelegate::paintGraph(QPainter* p, const QStyleOptionViewItem& opt,
                                  const QModelIndex& i) const {

    static const QColor colors[8] = { Qt::black, Qt::red, DARK_GREEN,
                                               Qt::blue, Qt::darkGray, BROWN,
                                               Qt::magenta, ORANGE };
    if (opt.state & QStyle::State_Selected)
        p->fillRect(opt.rect, opt.palette.highlight());
    else if (i.row() & 1)
        p->fillRect(opt.rect, opt.palette.alternateBase());
    else
        p->fillRect(opt.rect, opt.palette.base());


    Commit* commit = _repo->getAllCommits().at(i.row());

    if (!commit)
    {
        return;
    }

    p->save();
    p->setClipRect(opt.rect, Qt::IntersectClip);
    p->translate(opt.rect.topLeft());

    QBrush back = opt.palette.base();
    uint activeLane = commit->getRow();

    int x1 = 0, x2 = 0;

    int lw = 10;
    QColor activeColor = colors[activeLane % 8];
    if (opt.state & QStyle::State_Selected)
        activeColor = blend(activeColor, opt.palette.highlightedText().color(), 208);

    for (int i = 0; i < commit->getCurrentRowState()->size(); i++)
    {
        x1 = lw * (i);
        x2 = lw * (i + 1);
        bool active = commit->getRow() == i;

        QColor color = active ? activeColor : colors[i % 8];

        paintGraphLane(p, commit->getCurrentRowState()->at(i), x1, x2, color, activeColor, back);
    }
    p->restore();
}



void revViewDelegate::paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{

    p->setRenderHints(QPainter::Antialiasing);

    if (index.column() == 0)
        return paintGraph(p, opt, index);

    return QItemDelegate::paint(p, opt, index);
}
