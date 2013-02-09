#include "revviewdelegate.h"

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

//void revViewDelegate::diffTargetChanged(int row) {

//    if (diffTargetRow != row) {
//        diffTargetRow = row;
//        emit updateView();
//    }
//}

//const Rev* revViewDelegate::revLookup(int row, FileHistory** fhPtr) const {

//    ListView* lv = static_cast<ListView*>(parent());
//    FileHistory* fh = static_cast<FileHistory*>(lv->model());

//    if (lp->sourceModel())
//        fh = static_cast<FileHistory*>(lp->sourceModel());

//    if (fhPtr)
//        *fhPtr = fh;

//    return git->revLookup(lv->sha(row), fh);
//}

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
                                      const QColor& col, const QColor& activeCol, const QBrush& back) const {

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
/*    case JOIN:
    case JOIN_R:
    case HEAD:
    case HEAD_R:*/
    // TODO this needs to be sorted out atm I only
    // know this is a merge commit and not if which way the merge is heading eg. merge master to branch 1
    // or branch1 back to master
    case Commit::MERGE_COMMIT:
    {
        QConicalGradient gradient(CENTER_UR);
        gradient.setColorAt(0.375, col);
        gradient.setColorAt(0.625, activeCol);
        myPen.setBrush(gradient);
        p->setPen(myPen);
        p->drawArc(P_CENTER, DELTA_UR);
        break;
    }
    case Commit::BRANCH_COMMIT:
    //case JOIN_L:
    {
        QConicalGradient gradient(CENTER_UL);
        gradient.setColorAt(0.375, activeCol);
        gradient.setColorAt(0.625, col);
        myPen.setBrush(gradient);
        p->setPen(myPen);
        p->drawArc(P_CENTER, DELTA_UL);
        break;
    }
//    case TAIL:
//    case TAIL_R: {
//        QConicalGradient gradient(CENTER_DR);
//        gradient.setColorAt(0.375, activeCol);
//        gradient.setColorAt(0.625, col);
//        myPen.setBrush(gradient);
//        p->setPen(myPen);
//        p->drawArc(P_CENTER, DELTA_DR);
//        break;
//    }
    default:
        break;
    }

    myPen.setColor(col);
    p->setPen(myPen);

    // vertical line
    switch (type) {
    case Commit::NORMAL_COMMIT:
//    case ACTIVE:
//    case NOT_ACTIVE:
//    case MERGE_FORK:
//    case MERGE_FORK_R:
//    case MERGE_FORK_L:
//    case JOIN:
//    case JOIN_R:
//    case JOIN_L:
//    case CROSS:
        p->drawLine(P_90, P_270);
        break;
//    case HEAD_L:
    case Commit::BRANCH_COMMIT:
        p->drawLine(P_CENTER, P_270);
        break;
//    case TAIL_L:
//    case INITIAL:
//    case BOUNDARY:
//    case BOUNDARY_C:
//    case BOUNDARY_R:
//    case BOUNDARY_L:
    case Commit::MERGE_COMMIT:

        p->drawLine(P_90, P_CENTER);
        break;
    default:
        break;
    }

    myPen.setColor(activeCol);
    p->setPen(myPen);

    // horizontal line
    switch (type) {
//    case MERGE_FORK:
//    case JOIN:
//    case HEAD:
//    case TAIL:
//    case CROSS:
//    case CROSS_EMPTY:
//    case BOUNDARY_C:
    case Commit::MERGE_COMMIT:
    case Commit::BRANCH_MERGE_COMMIT:
        p->drawLine(P_180, P_0);
        break;
//    case MERGE_FORK_R:
//    case BOUNDARY_R:
    case Commit::BRANCH_COMMIT:
        p->drawLine(P_180, P_CENTER);
        break;
    case Commit::NORMAL_COMMIT:
//    case MERGE_FORK_L:
//    case HEAD_L:
//    case TAIL_L:
//    case BOUNDARY_L:
//        p->drawLine(P_CENTER, P_0);
        break;
    default:
        break;
    }

    // center symbol, e.g. rect or ellipse
    switch (type) {
    case Commit::BRANCH_COMMIT:
//    case ACTIVE:
//    case INITIAL:
//    case BRANCH:
        p->setPen(Qt::NoPen);
        p->setBrush(col);
        p->drawEllipse(R_CENTER);
        break;
    case Commit::MERGE_COMMIT:
//    case MERGE_FORK:
//    case MERGE_FORK_R:
//    case MERGE_FORK_L:
        p->setPen(Qt::NoPen);
        p->setBrush(col);
        p->drawRect(R_CENTER);
        break;
//    case UNAPPLIED:
//        // Red minus sign
//        p->setPen(Qt::NoPen);
//        p->setBrush(Qt::red);
//        p->drawRect(m - r, h - 1, d, 2);
//        break;
//    case APPLIED:
//        // Green plus sign
//        p->setPen(Qt::NoPen);
//        p->setBrush(DARK_GREEN);
//        p->drawRect(m - r, h - 1, d, 2);
//        p->drawRect(m - 1, h - r, 2, d);
//        break;
//    case BOUNDARY:
    case Commit::NORMAL_COMMIT:
        p->setBrush(back);
        p->drawEllipse(R_CENTER);
        break;
//    case BOUNDARY_C:
//    case BOUNDARY_R:
//    case BOUNDARY_L:
//        p->setBrush(back);
//        p->drawRect(R_CENTER);
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
        return;

    p->save();
    p->setClipRect(opt.rect, Qt::IntersectClip);
    p->translate(opt.rect.topLeft());

    // calculate lanes
//    if (commit->se== 0)
//        git->setLane(r->sha(), fh);

    QBrush back = opt.palette.base();
//    const QVector<int>& lanes(commit);
//    uint laneNum = lanes.count();
    uint activeLane = commit->getRow();

    int x1 = 0, x2 = 0;
    int maxWidth = opt.rect.width();
    int lw = 19; //laneWidth();
    QColor activeColor = colors[activeLane % 8];
    if (opt.state & QStyle::State_Selected)
        activeColor = blend(activeColor, opt.palette.highlightedText().color(), 208);
//    for (uint i = 0; i < 8 && x2 < maxWidth; i++) {

        x1 = lw * (commit->getRow() + 1);
        x2 = lw * (commit->getRow() + 2);


        QColor color = commit->getRow() == activeLane ? activeColor : colors[commit->getRow() % 8];
        paintGraphLane(p, commit->getCommitType(), x1, x2, color, activeColor, back);
//    }
    p->restore();
}



void revViewDelegate::paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{

    p->setRenderHints(QPainter::Antialiasing);

    if (index.column() == 0)
        return paintGraph(p, opt, index);

//    if (index.column() == LOG_COL)
//        return paintLog(p, opt, index);

    return QItemDelegate::paint(p, opt, index);
}
