#include "colorpicker.h"
#include "flynote.h"

#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>

QVector<QColor> ColorPicker::colorNotes =
{
    QColor("lightblue"),
    QColor("lightsteelblue"),
    QColor("lightgreen"),
    QColor("sandybrown"),
    QColor("pink"),
    QColor("#fafaa9"), // yellow
};

ColorPicker::ColorPicker(FlyNote *parent)
    : QWidget(parent)
    , parentFlynote(parent)
{

}

ColorPicker::~ColorPicker()
{

}

#include <QDebug>
void ColorPicker::mousePressEvent(QMouseEvent *evt)
{
    Q_UNUSED(evt);
    QPixmap pix = grab(QRect(evt->pos(),QSize(1, 1)));
    QImage img(pix.toImage());
    QColor c = img.pixel(0, 0);
    if (colorNotes.contains(c)){
        parentFlynote->setColor(c);
    } else if (c != QColor(240, 240, 240) /*bg*/) {
        // We click on the bg.darker(110) rect
        c = QColorDialog::getColor(colorNotes.first(), this, "Note color");
        if (c.isValid()){
            parentFlynote->setColor(c);
        }
    }
}

void ColorPicker::paintEvent(QPaintEvent *evt)
{
    Q_UNUSED(evt);

    QPainter painter(this);
    // Draw background
    QColor bg(240, 240, 240);
    painter.fillRect(rect(), QBrush(bg));
    // Draw default colors rect
    int padding = 4;
    QRectF r = rect();
    /** 6 default colors, follow by full color picker (2 rows / 4 columns)
      +-------------------+
      | |-||-||-|  +---+  |
      | |-||-||-|  +---+  |
      +-------------------+
     */
    double w = (r.width()-padding*2) / 4;
    double h = (r.height()-padding*2) / 2;
    int x, y, n=0;
    x = y = padding;
    for (const QColor& c: colorNotes){
        QRectF r_color(x, y, w-padding, h-padding);
        if (r_color.isValid()){
            painter.fillRect(r_color, QBrush(c));
            QFont f = painter.font();
            f.setPixelSize(int(r.height()/2-padding));
            painter.setFont(f);
            if (c == parentFlynote->getColor()){
                painter.setPen(c.lighter(110));
                painter.drawText(r_color, (Qt::AlignCenter|Qt::AlignHCenter), "✓");
            }
            n++;
            x += (w+padding);
            if (n%3 == 0){
                n = 0;
                x = padding;
                y += h+padding;
            }
        }
    }
    // Draw color dialog rect
    QRectF r_more(r.right()+padding-w, padding, w-padding*2, r.height()-padding*2);
    painter.fillRect(r_more, QBrush(bg.darker(110)));
    r_more.setHeight(r_more.height()-fontMetrics().height());
    painter.setPen(bg.darker(180));
    painter.drawText(r_more, (Qt::AlignCenter|Qt::AlignHCenter), "...");
}
