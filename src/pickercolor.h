#ifndef PICKERCOLOR_H
#define PICKERCOLOR_H

#include <QWidget>

class PickerColor : public QWidget
{
    Q_OBJECT
public:
    explicit PickerColor(QWidget *parent = nullptr);
    ~PickerColor() override;
    static QVector<QColor> colorNotes;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *evt) override;

};

#endif // PICKERCOLOR_H
