#ifndef FLYNOTE_H
#define FLYNOTE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QSizeGrip>
#include <QTextEdit>
#include <QPropertyAnimation>

class FlyNoteTitle;
class PickerColor;

class FlyNote : public QWidget
{
    Q_OBJECT
public:
    explicit FlyNote(const QString& title, const QString& text);
    explicit FlyNote(const QColor& color, const QString& title, const QString& text);
    ~FlyNote() override;
    QColor getColor() const;

signals:

public slots:
    void displayPickerColor();
    void invertAnimationSettings();

protected:
    void paintEvent(QPaintEvent *evt) override;
    void resizeEvent(QResizeEvent *evt) override;

private:
    void init();
    QVBoxLayout *vLayout;
    QTextEdit *editText;
    QColor color;
    QSizeGrip sizeGrip;
    FlyNoteTitle *flynoteTitle;
    PickerColor *pickerColor;
    QPropertyAnimation *pickerAnimation;

};

#endif // FLYNOTE_H
