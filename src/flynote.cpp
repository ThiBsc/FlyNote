#include "flynote.h"
#include "flynotetitle.h"
#include "colorpicker.h"
#include "notelistmodel.h"

#include <QPainter>
#include <QEvent>

FlyNote::FlyNote(const QString &title, const QString &text)
    : QWidget (nullptr)
    , vLayout(new QVBoxLayout(this))
    , editText(new QTextEdit(text, this))
    , color(QColor("lightblue"))
    , sizeGrip(this)
    , flynoteTitle(new FlyNoteTitle(title, this))
    , colorPicker(new ColorPicker(this))
    , pickerAnimation(new QPropertyAnimation(colorPicker, "minimumHeight", this))
    , focusAlphaAnimation(new QPropertyAnimation(this, "alpha"))
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
    , colorPicker(new ColorPicker(this))
    , pickerAnimation(new QPropertyAnimation(colorPicker, "minimumHeight", this))
    , focusAlphaAnimation(new QPropertyAnimation(this, "alpha"))
{
    init();
}

FlyNote::~FlyNote()
{
    NoteListModel *model = NoteListModel::getInstance();
    model->disableNote(this);

    delete flynoteTitle;
    delete editText;
    delete focusAlphaAnimation;
    delete pickerAnimation;
    delete colorPicker;
    delete vLayout;
}

void FlyNote::setTitle(const QString &title)
{
    flynoteTitle->setTitle(title);
}

QString FlyNote::getTitle() const
{
    return flynoteTitle->getTitle();
}

void FlyNote::setContent(const QString &content)
{
    editText->setText(content);
}

QString FlyNote::getContent() const
{
    return editText->toPlainText();
}

QColor FlyNote::getColor() const
{
    return color;
}

void FlyNote::setColor(const QColor &color)
{
    this->color = color;
    flynoteTitle->updateButtons();
    update();
    emit noteAppearanceChanged(this);
}

int FlyNote::getAlpha() const
{
    return color.alpha();
}

void FlyNote::setAlpha(int alpha)
{
    color.setAlpha(alpha);
    update();
}

void FlyNote::displayPickerColor()
{
    pickerAnimation->start();
}

void FlyNote::invertAnimationSettings()
{
    QVariant tmp = pickerAnimation->startValue();
    pickerAnimation->setStartValue(pickerAnimation->endValue());
    pickerAnimation->setEndValue(tmp);
}

void FlyNote::emitNoteAppearanceChanged()
{
    emit noteAppearanceChanged(this);
}

/** trick:
 * https://stackoverflow.com/questions/14377141/in-qt-what-qevent-means-loses-window-focus-regain-window-focus-set-transpare
 */
void FlyNote::changeEvent(QEvent *evt)
{
    QWidget::changeEvent(evt);
    if (evt->type() == QEvent::ActivationChange){
        if(isActiveWindow()){
            // widget is now active
            focusAlphaAnimation->setStartValue(125);
            focusAlphaAnimation->setEndValue(255);
            focusAlphaAnimation->start();
        } else {
            // widget is now inactive
            focusAlphaAnimation->setStartValue(255);
            focusAlphaAnimation->setEndValue(125);
            focusAlphaAnimation->start();
        }
    }
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
    sizeGrip.resize(32, 32);
    sizeGrip.move(width()-sizeGrip.width(), height()-sizeGrip.height());
}

void FlyNote::init()
{
    // Link this to the NoteListModel
    NoteListModel *model = NoteListModel::getInstance();
    //model->insertNote(model->rowCount(), this);
    connect(this, SIGNAL(noteAppearanceChanged(FlyNote*)), model, SLOT(updateNote(FlyNote*)));
    connect(flynoteTitle, SIGNAL(titleChanged(const QString&)), this, SLOT(emitNoteAppearanceChanged()));
    connect(editText, SIGNAL(textChanged()), this, SLOT(emitNoteAppearanceChanged()));

    // Set transparent and no border textedit
    QPalette pal = editText->palette();
    pal.setBrush(QPalette::Base, Qt::transparent);
    editText->setPalette(pal);
    editText->setFrameStyle(QFrame::NoFrame);
    editText->moveCursor(QTextCursor::End);

    vLayout->setContentsMargins(5,5,5,5);
    vLayout->addWidget(flynoteTitle);
    vLayout->addWidget(colorPicker);
    vLayout->addWidget(editText, 1);
    setLayout(vLayout);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // Animation settings
    pickerAnimation->setDuration(500);
    pickerAnimation->setStartValue(0);
    pickerAnimation->setEndValue(75);
    focusAlphaAnimation->setDuration(500);
    connect(flynoteTitle, SIGNAL(wantPickerColor()), this, SLOT(displayPickerColor()));
    connect(pickerAnimation, SIGNAL(finished()), this, SLOT(invertAnimationSettings()));

    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_NoSystemBackground);
    //setAttribute(Qt::WA_PaintOnScreen);
    //setAttribute(Qt::WA_TransparentForMouseEvents);

    //setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    resize(256, 256);
}
