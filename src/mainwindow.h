#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QVBoxLayout>
#include <QUdpSocket>
#include <QComboBox>
#include <QSystemTrayIcon>
#include <QToolButton>

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
    void showSystemTrayMessage(const QString &title, const QString &message, int msecs = 10000);

public slots:
    void addUser(const QString &user, const QHostAddress &address);
    void deleteUser(const QString &user, const QHostAddress &address);
    void editNote();
    void deleteNote();
    void sendNoteToUser(QAction *action);
    void sendNoteToBroadcast();
    void writeDatagramToBroadcast(const QByteArray &datagram);
    void receiveData();
    void systemTrayActivated(QSystemTrayIcon::ActivationReason reason);
#if defined (_WIN32) || defined (_WIN64)
   void changeWindowsNetwork(const QString &network);
#endif

private:
    Ui::MainWindow *ui;
    QMenu *stMenu;
    QSystemTrayIcon *stIcon;

    const quint16 broadcast_port = 45454;
    QVBoxLayout *vLayout;
    QListView *noteListView;
    NoteListModel *notelistModel;
    QMenu *usersMenu;
    QToolButton *usersButton;
    QUdpSocket *udpSender, *udpReceiver;
    ushort volatile sendedByMe;
#if defined (_WIN32) || defined (_WIN64)
    QComboBox *networkCbx;
    QHostAddress broadcastAddress;
#endif

};

#endif // MAINWINDOW_H
