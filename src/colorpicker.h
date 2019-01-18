#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>

class ColorPicker : public QWidget
{
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = nullptr);
    ~ColorPicker() override;
    static QVector<QColor> colorNotes;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *evt) override;

};

#endif // COLORPICKER_H
