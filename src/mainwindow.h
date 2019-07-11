#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QVBoxLayout>
#include <QUdpSocket>
#include <QComboBox>

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
    void deleteNote();
    void sendBroadcastNote();
    void receiveBroadcastNote();

private:
    Ui::MainWindow *ui;

    const quint16 broadcast_port = 45454;
    QVBoxLayout *vLayout;
    QListView *noteListView;
    NoteListModel *notelistModel;
    QUdpSocket *udpSender, *udpReceiver;
    bool volatile sendedByMe;
#ifdef _WIN32
    QComboBox *networkCbx;
#endif

};

#endif // MAINWINDOW_H
