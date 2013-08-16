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
#include "mainwindowrevview.h"

#include <QDebug>

revViewDelegate::revViewDelegate( AcGit::Repository* repo, MainWindowRevView *revView,
QObject *parent) :
    QItemDelegate(parent)
{
    this->repo = repo;
    this->revView = revView;
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
                                      const QColor& col, const QColor& activeCol, const QBrush& back, bool firstCommit) const
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
    case AcGit::Graph::MERGE_COMMIT_DOWN:
    case AcGit::Graph::MERGE_COMMIT_DOWN_H:
    case AcGit::Graph::BRANCH_MERGE_COMMIT_DOWN:
    case AcGit::Graph::BRANCH_MERGE_COMMIT_DOWN_H:
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
    case AcGit::Graph::BRANCH_COMMIT_UP:
    case AcGit::Graph::BRANCH_COMMIT_UP_H:
    case AcGit::Graph::BRANCH_MERGE_COMMIT_UP:
    case AcGit::Graph::BRANCH_MERGE_COMMIT_UP_H:
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
    case AcGit::Graph::BRANCH_MERGE_COMMIT_BOTH:
    case AcGit::Graph::BRANCH_MERGE_COMMIT_BOTH_H:
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
    case AcGit::Graph::NORMAL_COMMIT:
    case AcGit::Graph::BRANCH_COMMIT:
    case AcGit::Graph::MERGE_COMMIT:
    case AcGit::Graph::BRANCH_MERGE_COMMIT:
    case AcGit::Graph::NO_COMMIT:
    case AcGit::Graph::NO_COMMIT_H:
        if (firstCommit)
            p->drawLine(P_CENTER, P_270);
        else
            p->drawLine(P_90, P_270);
        break;
    default:
        break;
    }

    myPen.setColor(activeCol);
    p->setPen(myPen);

    // horizontal line
    switch (type) {
    case AcGit::Graph::MERGE_COMMIT_DOWN_H:
    case AcGit::Graph::BRANCH_MERGE_COMMIT_BOTH_H:
    case AcGit::Graph::BRANCH_MERGE_COMMIT_DOWN_H:
    case AcGit::Graph::BRANCH_MERGE_COMMIT_UP_H:
    case AcGit::Graph::BRANCH_COMMIT_UP_H:
    case AcGit::Graph::NO_COMMIT_H:
    case AcGit::Graph::EMPTY_LANE_H:
        p->drawLine(P_180, P_0);
        //p->drawLine(P_CENTER, P_0);
        break;
    case AcGit::Graph::MERGE_COMMIT:
    case AcGit::Graph::BRANCH_COMMIT:
    case AcGit::Graph::BRANCH_MERGE_COMMIT:
        p->drawLine(P_CENTER, P_0);
    default:
        break;
    }

    // center symbol, e.g. rect or ellipse
    switch (type) {
    case AcGit::Graph::MERGE_COMMIT:
    case AcGit::Graph::BRANCH_COMMIT:
    case AcGit::Graph::BRANCH_MERGE_COMMIT:
        p->setPen(Qt::NoPen);
        p->setBrush(col);
        p->drawRect(R_CENTER);
        break;
    case AcGit::Graph::NORMAL_COMMIT:
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

const bool revViewDelegate::hasWorkingDirectoryChanges() const
{
    return revView->hasWorkingDirectoryChanges();
}

void revViewDelegate::paintGraph(QPainter* p, const QStyleOptionViewItem& opt,
                                  const QModelIndex& index) const {

    int row = index.row();

    if (row == 0 && hasWorkingDirectoryChanges())
    {
        return;
    }

    bool firstCommit = index.row() == 0;
    if (row == 1 && hasWorkingDirectoryChanges())
    {
        firstCommit = true;
        row--;
    }

    AcGit::Commit *commit = nullptr;
    if (row >= 0 && row < repo->CommitsAgent()->getAllCommits()->length())
    {
        commit = repo->CommitsAgent()->getAllCommits()->at(row);
    }

    if (!commit || commit->getGraph() == nullptr)
    {
        return;
    }

    AcGit::Graph *graph = commit->getGraph();


    static const QColor colors[8] = { Qt::black, Qt::red, DARK_GREEN,
                                               Qt::blue, Qt::darkGray, BROWN,
                                               Qt::magenta, ORANGE };
    if (opt.state & QStyle::State_Selected)
        p->fillRect(opt.rect, opt.palette.highlight());
    else if (index.row() & 1)
        p->fillRect(opt.rect, opt.palette.alternateBase());
    else
        p->fillRect(opt.rect, opt.palette.base());

    p->save();
    p->setClipRect(opt.rect, Qt::IntersectClip);
    p->translate(opt.rect.topLeft());

    QBrush back = opt.palette.base();
    uint activeLane = graph->activeRowNumber();

    int laneColStart = 0, laneColEnd = 0;
    int lanewidth = 10;

    QColor activeColor = colors[activeLane % 8];

    if (opt.state & QStyle::State_Selected)
        activeColor = blend(activeColor, opt.palette.highlightedText().color(), 208);


    for (int graphColIndex = 0; graphColIndex < graph->rowState()->size(); graphColIndex++)
    {
        laneColStart = lanewidth * (graphColIndex);
        laneColEnd = lanewidth * (graphColIndex + 1);
        bool active = graph->activeRowNumber() == graphColIndex;

        QColor color = active ? activeColor : colors[graphColIndex % 8];

        paintGraphLane(p, graph->rowState()->at(graphColIndex), laneColStart, laneColEnd, color, activeColor, back, firstCommit);
    }
    p->restore();
}


void revViewDelegate::paintShort(QPainter* p, QStyleOptionViewItem opt,
                                const QModelIndex& index) const
{

    int row = index.row();

    
    if (hasWorkingDirectoryChanges() == true)
    {
        // to allow for off commit array starting at 0
        row -= 1;
    }

    AcGit::Commit *commit = nullptr;
    if (row >= 0 && row < repo->CommitsAgent()->getAllCommits()->length() )
        commit = repo->CommitsAgent()->getAllCommits()->at(row);

    if (!commit)
    {
        QItemDelegate::paint(p, opt, index);
        return;
    }

    QPixmap* pm = getRefsPixmap(commit, opt);

    if (!pm) {
        QItemDelegate::paint(p, opt, index);
        return;
    }

    QStyleOptionViewItem newOpt(opt); // we need a copy
    p->drawPixmap(newOpt.rect.x(), newOpt.rect.y() + 6, *pm);
    newOpt.rect.adjust(pm->width(), 0, 0, 0);
    delete pm;

    QItemDelegate::paint(p, newOpt, index);
}

QPixmap* revViewDelegate::getRefsPixmap(AcGit::Commit *commit, QStyleOptionViewItem& opt) const
{
    QPixmap* pm = new QPixmap(); // must be deleted by caller

    AcGit::IBranches *branchAgent = repo->BranchAgent();
    AcGit::ITags *tagAgent = repo->TagsAgent();

    QList<AcGit::Branch *> branches = branchAgent->lookupBranch(commit);
    foreach (AcGit::Branch *branch, branches)
    {
        opt.palette.setColor(QPalette::Window, QColor(Qt::green));

        addTextPixmap(&pm, branch->getBranchName(), opt);
    }

    QList<AcGit::Tag *> tags = tagAgent->lookupTag(commit);
    foreach (AcGit::Tag *tag, tags)
    {

        opt.palette.setColor(QPalette::Window, QColor(Qt::red));

        addTextPixmap(&pm, tag->name(), opt);
    }

    return pm;
}


void revViewDelegate::addTextPixmap(QPixmap** pp, QString data, const QStyleOptionViewItem& opt) const {

    QPixmap* pm = *pp;
    int ofs = pm->isNull() ? 0 : pm->width() + 2;
    int spacing = 2;
    QFontMetrics fm(opt.font);
    int pw = fm.boundingRect(data).width() + 2 * (spacing + int(opt.font.bold()));
    int ph = fm.height() - 1; // leave vertical space between two consecutive tags

    QPixmap* newPm = new QPixmap(ofs + pw, ph);
    QPainter p;
    p.begin(newPm);
    if (!pm->isNull()) {
        newPm->fill(opt.palette.base().color());
        p.drawPixmap(0, 0, *pm);
    }
    p.setPen(opt.palette.color(QPalette::WindowText));
    p.setBrush(opt.palette.color(QPalette::Window));
    p.setFont(opt.font);
    p.drawRect(ofs, 0, pw - 1, ph - 1);
    p.drawText(ofs + spacing, fm.ascent(), data);
    p.end();

    delete pm;
    *pp = newPm;
}

void revViewDelegate::paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{

    p->setRenderHints(QPainter::Antialiasing);

    if (index.column() == 0)
    {
        return paintGraph (p, opt, index);
    }

    if (index.column() == 1)
    {
        return paintShort (p, opt, index);
    }

    return QItemDelegate::paint(p, opt, index);
}
