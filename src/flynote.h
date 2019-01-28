#ifndef FLYNOTE_H
#define FLYNOTE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QSizeGrip>
#include <QTextEdit>
#include <QPropertyAnimation>

class FlyNoteTitle;
class ColorPicker;

class FlyNote : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int alpha READ getAlpha WRITE setAlpha)

public:
    explicit FlyNote(const QString& title, const QString& text);
    explicit FlyNote(const QColor& color, const QString& title, const QString& text);
    ~FlyNote() override;
    QString getTitle() const;
    QColor getColor() const;
    void setColor(const QColor& color);
    int getAlpha() const;
    void setAlpha(int alpha);

signals:
    void noteAppearanceChanged(FlyNote *note);

public slots:
    void displayPickerColor();
    void invertAnimationSettings();
    void emitNoteAppearanceChanged();

protected:
    void changeEvent(QEvent *evt) override;
    void paintEvent(QPaintEvent *evt) override;
    void resizeEvent(QResizeEvent *evt) override;

private:
    void init();
    QVBoxLayout *vLayout;
    QTextEdit *editText;
    QColor color;
    QSizeGrip sizeGrip;
    FlyNoteTitle *flynoteTitle;
    ColorPicker *colorPicker;
    QPropertyAnimation *pickerAnimation, *focusAlphaAnimation;

};

#endif // FLYNOTE_H
