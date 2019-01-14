#include "flynotebutton.h"
#include "flynote.h"

#include <QPainter>

FlyNoteButton::FlyNoteButton(ButtonType type, FlyNote *parent)
    : QAbstractButton (parent)
    , type(type)
    , state(NORMAL)
    , pixNormal(nullptr)
    , pixHovered(nullptr)
    , pixClicked(nullptr)
    , flynoteParent(parent)
{

}

FlyNoteButton::~FlyNoteButton()
{
    delete pixNormal;
    delete pixHovered;
    delete pixClicked;
}

void FlyNoteButton::enterEvent(QEvent *evt)
{
    Q_UNUSED(evt);
    state = HOVERED;
    update();
}

void FlyNoteButton::leaveEvent(QEvent *evt)
{
    Q_UNUSED(evt);
    state = NORMAL;
}

void FlyNoteButton::mousePressEvent(QMouseEvent *evt)
{
    QAbstractButton::mousePressEvent(evt);
    state = CLICKED;
    update();
}

void FlyNoteButton::mouseReleaseEvent(QMouseEvent *evt)
{
    QAbstractButton::mouseReleaseEvent(evt);
    if (type == CLOSENOTE){
        delete flynoteParent;
    } else {
        if (underMouse()){
            state = HOVERED;
        } else {
            state = NORMAL;
        }
        update();
    }
}

void FlyNoteButton::paintEvent(QPaintEvent *evt)
{
    Q_UNUSED(evt);

    QPainter painter(this);
    if (isEnabled()){
        switch (state) {
        case NORMAL:
            if (pixNormal != nullptr) painter.drawPixmap(0, 0, *pixNormal);
            break;
        case HOVERED:
            if (pixHovered != nullptr) painter.drawPixmap(0, 0, *pixHovered);
            break;
        case CLICKED:
            if (pixClicked != nullptr) painter.drawPixmap(0, 0, *pixClicked);
            break;
        }
    } else {
        if (pixNormal != nullptr) painter.drawPixmap(0, 0, *pixNormal);
    }
}

void FlyNoteButton::resizeEvent(QResizeEvent *evt)
{
    Q_UNUSED(evt);
    initPixmaps();
}

void FlyNoteButton::initPixmaps()
{
    initPixmap(&pixNormal);
    initPixmap(&pixHovered);
    initPixmap(&pixClicked);

    switch (type) {
    case NEWNOTE:
        initNew();
        break;
    case COLORNOTE:
        initColor();
        break;
    case CLOSENOTE:
        initClose();
        break;
    }
}

void FlyNoteButton::initPixmap(QPixmap **pixmap)
{
    delete *pixmap;
    *pixmap = new QPixmap(32, 32);
    (*pixmap)->fill(Qt::transparent);
}

void FlyNoteButton::initNew()
{
    // + (filled)
    QPainter painter;
    QColor color = flynoteParent->getColor().lighter(110);
    QRectF r = rect();
    QPen pen(color);
    pen.setWidthF(r.width()/4);
    /* Normal */
    painter.begin(pixNormal);
    painter.setPen(pen);
    painter.setBrush(QBrush(color));
    painter.drawLine(QPointF(r.width()/2, 0), QPointF(r.width()/2, r.height()));
    painter.drawLine(QPointF(0, r.height()/2), QPointF(width(), r.height()/2));
    painter.end();
    /* End normal */
    /* Hovered */
    pen.setColor(color.darker(105));
    painter.begin(pixHovered);
    painter.setPen(pen);
    painter.setBrush(QBrush(color.darker(105)));
    painter.drawLine(QPointF(r.width()/2, 0), QPointF(r.width()/2, r.height()));
    painter.drawLine(QPointF(0, r.height()/2), QPointF(width(), r.height()/2));
    painter.end();
    /* End hovered */
    /* Normal */
    pen.setColor(color.darker(110));
    painter.begin(pixClicked);
    painter.setPen(pen);
    painter.setBrush(QBrush(color.darker(110)));
    painter.drawLine(QPointF(r.width()/2, 0), QPointF(r.width()/2, r.height()));
    painter.drawLine(QPointF(0, r.height()/2), QPointF(width(), r.height()/2));
    painter.end();
    /* End normal */
}

void FlyNoteButton::initColor()
{
    // o (filled)
    QPainter painter;
    QColor color = flynoteParent->getColor().lighter(110);
    QRect r = rect();
    /* Normal */
    painter.begin(pixNormal);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(color));
    painter.setBrush(QBrush(color));
    painter.drawEllipse(r);
    painter.end();
    /* End normal */
    /* Hovered */
    painter.begin(pixHovered);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(color.darker(105)));
    painter.setBrush(QBrush(color.darker(105)));
    painter.drawEllipse(r);
    painter.end();
    /* End hovered */
    /* Normal */
    painter.begin(pixClicked);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(color.darker(110)));
    painter.setBrush(QBrush(color.darker(110)));
    painter.drawEllipse(r);
    painter.end();
    /* End normal */
}

void FlyNoteButton::initClose()
{
    // x (filled)
    QPainter painter;
    QColor color = flynoteParent->getColor().lighter(110);
    QRectF r = rect();
    QPen pen(color);
    pen.setWidthF(r.width()/4);
    /* Normal */
    painter.begin(pixNormal);
    painter.setPen(pen);
    painter.setBrush(QBrush(color));
    painter.drawLine(QPointF(0, 0), QPointF(r.width(), r.height()));
    painter.drawLine(QPointF(0, r.height()), QPointF(r.width(), 0));
    painter.end();
    /* End normal */
    /* Hovered */
    pen.setColor(color.darker(105));
    painter.begin(pixHovered);
    painter.setPen(pen);
    painter.setBrush(QBrush(color.darker(105)));
    painter.drawLine(QPointF(0, 0), QPointF(r.width(), r.height()));
    painter.drawLine(QPointF(0, r.height()), QPointF(r.width(), 0));
    painter.end();
    /* End hovered */
    /* Normal */
    pen.setColor(color.darker(110));
    painter.begin(pixClicked);
    painter.setPen(pen);
    painter.setBrush(QBrush(color.darker(110)));
    painter.drawLine(QPointF(0, 0), QPointF(r.width(), r.height()));
    painter.drawLine(QPointF(0, r.height()), QPointF(r.width(), 0));
    painter.end();
    /* End normal */
}
