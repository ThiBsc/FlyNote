#include "flynote.h"
#include "flynotetitle.h"

#include <QPainter>

QVector<QColor> FlyNote::colorNotes =
{
    QColor("lightblue"),
    QColor("lightgreen"),
    QColor("pink"),
    QColor("sandybrown"),
    QColor("lightsteelblue"),
};

FlyNote::FlyNote(const QString &title, const QString &text)
    : QWidget (nullptr)
    , vLayout(new QVBoxLayout(this))
    , editText(new QTextEdit(text, this))
    , color(QColor("lightblue"))
    , sizeGrip(this)
    , flynoteTitle(new FlyNoteTitle(title, this))
{
    init();
}

FlyNote::FlyNote(const QColor &color, const QString &title, const QString &text)
    : QWidget (nullptr)
    , vLayout(new QVBoxLayout(this))
    , editText(new QTextEdit(text, this))
    , color(color)
    , sizeGrip(this)
    , flynoteTitle(new FlyNoteTitle(title, this))
{
    init();
}

FlyNote::~FlyNote()
{
    delete flynoteTitle;
    delete editText;
    delete vLayout;
}

QColor FlyNote::getColor() const
{
    return color;
}

void FlyNote::paintEvent(QPaintEvent *evt)
{
    Q_UNUSED(evt);

    QPainter painter(this);
    int separation = flynoteTitle->height() + vLayout->contentsMargins().top()*2;
    painter.setPen(Qt::NoPen); // No stroke
    // title
    painter.setBrush(color.darker(105));
    painter.drawRect(0, 0, width(), separation);
    // text
    painter.setBrush(color);
    painter.drawRect(0, separation, width(), height());
}

void FlyNote::resizeEvent(QResizeEvent *evt)
{
    Q_UNUSED(evt);
    sizeGrip.move(width()-sizeGrip.width(), height()-sizeGrip.height());
}

void FlyNote::init()
{
    // Set transparent and no border textedit
    QPalette pal = editText->palette();
    pal.setBrush(QPalette::Base, Qt::transparent);
    editText->setPalette(pal);
    editText->setFrameStyle(QFrame::NoFrame);
    editText->moveCursor(QTextCursor::End);

    vLayout->setContentsMargins(5,5,5,5);
    vLayout->addWidget(flynoteTitle);
    vLayout->addWidget(editText, 1);
    setLayout(vLayout);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    resize(256, 256);
}
