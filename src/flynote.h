#ifndef FLYNOTE_H
#define FLYNOTE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QSizeGrip>
#include <QTextEdit>

class FlyNoteTitle;

class FlyNote : public QWidget
{
    Q_OBJECT
public:
    explicit FlyNote(const QString& title, const QString& text);
    explicit FlyNote(const QColor& color, const QString& title, const QString& text);
    ~FlyNote() override;
    QColor getColor() const;
    static QVector<QColor> colorNotes;

signals:

public slots:

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

};

#endif // FLYNOTE_H
