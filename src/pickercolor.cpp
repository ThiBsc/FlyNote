#include "pickercolor.h"

#include <QPainter>

QVector<QColor> PickerColor::colorNotes =
{
    QColor("lightblue"),
    QColor("lightgreen"),
    QColor("pink"),
    QColor("sandybrown"),
    QColor("lightsteelblue"),
    QColor(255, 255, 102), // yellow
};

PickerColor::PickerColor(QWidget *parent)
    : QWidget(parent)
{

}

PickerColor::~PickerColor()
{

}

void PickerColor::paintEvent(QPaintEvent *evt)
{
    Q_UNUSED(evt);

    QPainter painter(this);
    // Draw background
    painter.fillRect(rect(), QBrush(QColor(240, 240, 240)));
}
