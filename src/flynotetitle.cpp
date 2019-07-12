#include "flynotetitle.h"
#include "flynote.h"
#include "flynotebutton.h"
#include "colorpicker.h"
#include "notelistmodel.h"

#include <QMouseEvent>

FlyNoteTitle::FlyNoteTitle(const QString &title, FlyNote *parent)
    : QWidget (parent)
    , hLayout(new QHBoxLayout(this))
    , flynoteParent(parent)
    , btnNew(new FlyNoteButton(FlyNoteButton::NEWNOTE, flynoteParent))
    , btnColor(new FlyNoteButton(FlyNoteButton::COLORNOTE, flynoteParent))
    , btnClose(new FlyNoteButton(FlyNoteButton::CLOSENOTE, flynoteParent))
    , editTitle(new QLineEdit(title, this))
    , pDiff(0, 0)
{
    // Set transparent and no border lineedit
    QPalette pal = editTitle->palette();
    pal.setBrush(QPalette::Base, Qt::transparent);
    pal.setBrush(QPalette::Disabled, QPalette::Text, Qt::black);
    editTitle->setPalette(pal);
    editTitle->setFrame(false);
    editTitle->setEnabled(false);

    hLayout->setContentsMargins(0,0,0,0);
    hLayout->addWidget(btnNew);
    hLayout->addWidget(editTitle, 1);
    hLayout->addWidget(btnColor);
    hLayout->addWidget(btnClose);
    btnNew->setMinimumSize(12, 12);
    btnColor->setMinimumSize(12, 12);
    btnClose->setMinimumSize(12, 12);

    connect(btnNew, SIGNAL(clicked()), this, SLOT(openNewNote()));
    connect(btnColor, SIGNAL(clicked()), this, SLOT(openColor()));
    // Manage the validation of the new title
    connect(editTitle, SIGNAL(textChanged(const QString&)), this, SLOT(emitTitleChanged(const QString&)));
    connect(editTitle, SIGNAL(editingFinished()), this, SLOT(disableTitleAfterEdit()));
}

FlyNoteTitle::~FlyNoteTitle()
{
    delete btnNew;
    delete btnClose;
    delete btnColor;
    delete editTitle;
    delete hLayout;
}

void FlyNoteTitle::updateButtons()
{
    btnNew->updateButtons();
    btnClose->updateButtons();
    btnColor->updateButtons();
}

void FlyNoteTitle::setTitle(const QString &title)
{
    editTitle->setText(title);
}

QString FlyNoteTitle::getTitle() const
{
    return editTitle->text();
}

void FlyNoteTitle::openNewNote()
{
    FlyNote *f = new FlyNote(ColorPicker::getRandomColor(), "New note", "");
    NoteListModel::instance->insertNote(NoteListModel::instance->rowCount(), f);
    f->show();
}

void FlyNoteTitle::openColor()
{
    emit wantPickerColor();
}

void FlyNoteTitle::disableTitleAfterEdit()
{
    editTitle->setEnabled(false);
}

void FlyNoteTitle::emitTitleChanged(const QString &title)
{
    emit titleChanged(title);
}

void FlyNoteTitle::mousePressEvent(QMouseEvent *evt)
{
    pDiff = evt->pos();
    setCursor(Qt::ClosedHandCursor);
}

void FlyNoteTitle::mouseReleaseEvent(QMouseEvent *evt)
{
    Q_UNUSED(evt);
    setCursor(Qt::ArrowCursor);
}

void FlyNoteTitle::mouseMoveEvent(QMouseEvent *evt)
{
    QPoint p = evt->globalPos();
    window()->move(p - pDiff);
}

void FlyNoteTitle::mouseDoubleClickEvent(QMouseEvent *evt)
{
    Q_UNUSED(evt);
    editTitle->setEnabled(true);
    editTitle->selectAll();
    editTitle->setCursorPosition(editTitle->text().length());
}
