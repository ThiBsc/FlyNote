#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QVBoxLayout>

#include "flynote.h"
#include "notelistmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void editNote();

private:
    Ui::MainWindow *ui;

    QVBoxLayout *vLayout;
    QListView *noteListView;
    NoteListModel *notelistModel;

    QAction *actAddNote, *actEditNote;

};

#endif // MAINWINDOW_H
