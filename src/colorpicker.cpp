#include "colorpicker.h"

#include <QPainter>

QVector<QColor> ColorPicker::colorNotes =
{
    QColor("lightblue"),
    QColor("lightgreen"),
    QColor("pink"),
    QColor("sandybrown"),
    QColor("lightsteelblue"),
    QColor(255, 255, 102), // yellow
};

ColorPicker::ColorPicker(QWidget *parent)
    : QWidget(parent)
{

}

ColorPicker::~ColorPicker()
{

}

void ColorPicker::paintEvent(QPaintEvent *evt)
{
    Q_UNUSED(evt);

    QPainter painter(this);
    // Draw background
    painter.fillRect(rect(), QBrush(QColor(240, 240, 240)));
}
