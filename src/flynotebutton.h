#ifndef FLYNOTEBUTTON_H
#define FLYNOTEBUTTON_H

#include <QAbstractButton>
#include <QPixmap>

class FlyNote;

/**
 * @brief The FlyNoteButton class
 * https://qt.developpez.com/tutoriels/braindeadbzh/customwindow/#L3-4
 */
class FlyNoteButton : public QAbstractButton
{
    Q_OBJECT
public:
    enum ButtonType { NEWNOTE=0, COLORNOTE, CLOSENOTE };
    FlyNoteButton(ButtonType type, FlyNote *parent);
    ~FlyNoteButton() override;

protected:
    void enterEvent(QEvent *evt) override;
    void leaveEvent(QEvent *evt) override;
    void mousePressEvent(QMouseEvent *evt) override;
    void mouseReleaseEvent(QMouseEvent *evt) override;
    void paintEvent(QPaintEvent *evt) override;
    void resizeEvent(QResizeEvent *evt) override;

private:
    void initPixmaps();
    void initPixmap(QPixmap **pixmap);
    void initNew();
    void initColor();
    void initClose();
    enum ButtonState { NORMAL=0, HOVERED, CLICKED };
    ButtonType type;
    ButtonState state;
    QPixmap *pixNormal, *pixHovered, *pixClicked;
    FlyNote *flynoteParent;

};

#endif // FLYNOTEBUTTON_H
