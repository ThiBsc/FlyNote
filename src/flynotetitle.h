#ifndef FLYNOTETITLE_H
#define FLYNOTETITLE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class FlyNote;
class FlyNoteButton;

class FlyNoteTitle : public QWidget
{
    Q_OBJECT
public:
    explicit FlyNoteTitle(const QString& title, FlyNote *parent);
    ~FlyNoteTitle() override;
    void updateButtons();
    QString getTitle() const;

signals:
    void wantPickerColor();
    void titleChanged(const QString &newTitle);

public slots:
    void openNewNote();
    void openColor();
    void disableTitleAfterEdit();
    void emitTitleChanged(const QString &newTitle);

protected:
    /** https://qt.developpez.com/tutoriels/braindeadbzh/customwindow/ */
    void mousePressEvent(QMouseEvent *evt) override;
    void mouseReleaseEvent(QMouseEvent *evt) override;
    void mouseMoveEvent(QMouseEvent *evt) override;
    void mouseDoubleClickEvent(QMouseEvent *evt) override;

private:
    QHBoxLayout *hLayout;
    FlyNote *flynoteParent;
    FlyNoteButton *btnNew, *btnColor, *btnClose;
    QLineEdit *editTitle;
    QPoint pDiff;

};

#endif // FLYNOTETITLE_H
