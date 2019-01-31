#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>

class FlyNote;

class ColorPicker : public QWidget
{
    Q_OBJECT
public:
    explicit ColorPicker(FlyNote *parent = nullptr);
    ~ColorPicker() override;
    static QVector<QColor> colorNotes;
    static QColor getRandomColor();

signals:

public slots:

protected:
    void mousePressEvent(QMouseEvent *evt) override;
    void paintEvent(QPaintEvent *evt) override;

private:
    FlyNote *parentFlynote;

};

#endif // COLORPICKER_H
