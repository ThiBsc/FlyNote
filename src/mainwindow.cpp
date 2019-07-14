#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QHostInfo>

#ifdef _WIN32
// Manage the QHostAddress::Broadcast problem on windows
#include <QNetworkInterface>
#include <QNetworkConfigurationManager>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stMenu(new QMenu(this))
    , stIcon(new QSystemTrayIcon(this))
    , vLayout(new QVBoxLayout(centralWidget()))
    , noteListView(new QListView(this))
    , notelistModel(NoteListModel::instance)
    , usersMenu(new QMenu(this))
    , usersButton(new QToolButton(this))
    , udpSender(new QUdpSocket(this))
    , udpReceiver(new QUdpSocket(this))
    , sendedByMe(0)
#ifdef _WIN32
    , networkCbx(new QComboBox(this))
#endif
{
    ui->setupUi(this);

    setWindowTitle("Flynote");
    setWindowIcon(QIcon("://icons/note.png"));
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_QuitOnClose, false);

    noteListView->setModel(notelistModel);

    vLayout->setContentsMargins(QMargins());
    vLayout->addWidget(noteListView);
    centralWidget()->setLayout(vLayout);

    ui->mainToolBar->addAction(QIcon("://icons/add"), "New note", notelistModel, SLOT(addNote()));
    ui->mainToolBar->addAction(QIcon("://icons/delete"), "Delete note", this, SLOT(deleteNote()));
    ui->mainToolBar->addAction(QIcon("://icons/edit"), "Edit note", this, SLOT(editNote()));
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon("://icons/broadcast"), "Broadcast", this, SLOT(sendNoteToBroadcast()));
    ui->mainToolBar->addWidget(usersButton);
    //ui->mainToolBar->addAction(QIcon("://icons/send"), "Send", this, SLOT(sendNoteToUser()));
    usersButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    usersButton->setPopupMode(QToolButton::InstantPopup);
    usersButton->setIcon(QIcon("://icons/send"));
    usersButton->setToolTip("Send");
    usersButton->setMenu(usersMenu);
    usersButton->setEnabled(false);
    connect(usersMenu, SIGNAL(triggered(QAction*)), this, SLOT(sendNoteToUser(QAction*)));

    /** SystemTrayIcon Init*/
    stIcon->setToolTip("Flynote");
    stIcon->setIcon(windowIcon());

    stMenu->addAction("Show", this, SLOT(show()));
    stMenu->addAction("Quit", qApp, SLOT(quit()), Qt::QueuedConnection);
    stIcon->setContextMenu(stMenu);

    connect(stIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(systemTrayActivated(QSystemTrayIcon::ActivationReason)));
    stIcon->show();
    /** SystemTrayIcon Ok */

    /** Network management Init */
    // Test broadcast with 'ncat -u 192.168.0.255 45454
    const QString localHostName = QHostInfo::localHostName();
    ui->statusBar->showMessage(QString("Identified as %1").arg(localHostName));
#ifdef _WIN32
    // Add in a combobox all real broadcast address to avoid the use of QHostAddress::Broadcast in Windows
    ui->mainToolBar->addWidget(networkCbx);
    //networkCbx->setEditable(true);
    networkCbx->setToolTip("Broadcast address");
    QSet<QString> broadcast_set;
    QList<QNetworkInterface> networks = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& n : networks){
        QList<QNetworkAddressEntry> entries = n.addressEntries();
        for (const QNetworkAddressEntry& e : entries){
            QHostAddress bc = e.broadcast();
            if (!bc.isNull()){
                broadcast_set.insert(bc.toString());
            }
        }
    }
    networkCbx->addItems(broadcast_set.toList());
    broadcastAddress = QHostAddress(networkCbx->currentText());
    connect(networkCbx, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(changeWindowsNetwork(const QString&)));
#endif
    udpReceiver->bind(broadcast_port, QUdpSocket::ShareAddress);
    connect(udpReceiver, SIGNAL(readyRead()), this, SLOT(receiveData()));
    // Notify all users of my connection
    sendedByMe = 1;
#ifdef __linux__
    udpSender->writeDatagram(QString("user_join:%1").arg(localHostName).toUtf8(), QHostAddress::Broadcast, broadcast_port);
#else
    udpSender->writeDatagram(QString("user_join:%1").arg(localHostName).toUtf8(), QHostAddress(networkCbx->currentText()), broadcast_port);
#endif
    /** Network management Ok */

    resize(300, 350);
}

MainWindow::~MainWindow()
{
    sendedByMe = 1;
#ifdef __linux__
    udpSender->writeDatagram(QString("user_left:%1").arg(QHostInfo::localHostName()).toUtf8(), QHostAddress::Broadcast, broadcast_port);
#else
    udpSender->writeDatagram(QString("user_left:%1").arg(QHostInfo::localHostName()).toUtf8(), QHostAddress(networkCbx->currentText()), broadcast_port);
#endif
    udpSender->waitForBytesWritten(1000);

    delete ui;
#ifdef _WIN32
    delete networkCbx;
#endif
    delete usersMenu;
    delete usersButton;
    delete udpSender;
    delete udpReceiver;
    delete noteListView;
    delete vLayout;

    delete stMenu;
    delete stIcon;
}

void MainWindow::showSystemTrayMessage(const QString &title, const QString &message, int msecs)
{
    // display notification
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    stIcon->showMessage(title, message, windowIcon(), msecs);
#else
    stIcon->showMessage(title, message, QSystemTrayIcon::Information, msecs);
#endif
}

void MainWindow::addUser(const QString &user, const QHostAddress &address)
{
    QList<QAction*> actions = usersMenu->actions();
    auto action = std::find_if(actions.begin(), actions.end(), [address](QAction *a) -> bool { return QHostAddress(a->data().toString()) == address; });
    if (action == actions.end()){
        QAction *a = usersMenu->addAction(QIcon("://icons/user"), user);
        a->setData(address.toString());
    }
    usersButton->setEnabled(!usersMenu->isEmpty());
}

void MainWindow::deleteUser(const QString &user, const QHostAddress &address)
{
    Q_UNUSED(user)
    QList<QAction*> actions = usersMenu->actions();
    auto action = std::find_if(actions.begin(), actions.end(), [address](QAction *a) -> bool { return QHostAddress(a->data().toString()) == address; });
    if (action != actions.end()){
        usersMenu->removeAction(*action);
    }
    usersButton->setEnabled(!usersMenu->isEmpty());
}

void MainWindow::editNote()
{
    notelistModel->editNote(noteListView->currentIndex());
}

void MainWindow::deleteNote()
{
    notelistModel->deleteNote(noteListView->currentIndex());
}

void MainWindow::sendNoteToUser(QAction *action)
{
    // qDebug() << action->text();
    QModelIndex current_note = noteListView->currentIndex();
    if (current_note.isValid()){
        // get the note to broadcast
        QJsonObject json_note = notelistModel->jsonNote(current_note.row());
        json_note.insert("received", QString("user"));
        json_note.remove("address");
        QJsonDocument doc(json_note);

        QByteArray datagram_note = doc.toJson();
        datagram_note.prepend("note:");
        qDebug() << "Send to" << action->data().toString() << datagram_note;
        // sendedByMe = true; Not necessary because it's send to user
        udpSender->writeDatagram(datagram_note, QHostAddress(action->data().toString()), broadcast_port);
    }
}

/**
 * @brief MainWindow::sendBroadcastNote
 * https://doc.qt.io/qt-5/qtnetwork-broadcastsender-example.html
 */
void MainWindow::sendNoteToBroadcast()
{
    QModelIndex current_note = noteListView->currentIndex();
    if (current_note.isValid()){
        // get the note to broadcast
        QJsonObject json_note = notelistModel->jsonNote(current_note.row());
        json_note.insert("received", QString("broadcast"));
        json_note.remove("address");
        QJsonDocument doc(json_note);

        QByteArray datagram_note = doc.toJson();
        datagram_note.prepend("note:");
        qDebug() << "Send to broadcast" << datagram_note;
        sendedByMe = 1;
#ifdef __linux__
        udpSender->writeDatagram(datagram_note, QHostAddress::Broadcast, broadcast_port);
#else
        udpSender->writeDatagram(datagram_note, QHostAddress(networkCbx->currentText()), broadcast_port);
#endif
    }
}

/**
 * @brief MainWindow::receiveBroadcastNote
 * https://doc.qt.io/qt-5/qtnetwork-broadcastreceiver-example.html
 */
void MainWindow::receiveData()
{
    QByteArray datagram_note;
    QHostAddress sender;
    while (udpReceiver->hasPendingDatagrams()){
        datagram_note.resize(int(udpReceiver->pendingDatagramSize()));
        udpReceiver->readDatagram(datagram_note.data(), datagram_note.size(), &sender);
        // For each received datagram
        // qDebug() << datagram_note << sendedByMe;
        if (sendedByMe){
            sendedByMe--;
        } else {
            if (datagram_note.startsWith("add_me:")){
                // Add to user
                QString user = QString(datagram_note.split(':').last());
                addUser(user, sender);
            } else if (datagram_note.startsWith("user_join:")){
                QString user = QString(datagram_note.split(':').last());
                showSystemTrayMessage("Flynote", QString("%1 join").arg(user), 2000);
                addUser(user, sender);
                // Send a reply to say that he is not alone
                udpSender->writeDatagram(QString("add_me:%1").arg(QHostInfo::localHostName()).toUtf8(), sender, broadcast_port);
            } else if (datagram_note.startsWith("user_left:")) {
                QString user = QString(datagram_note.split(':').last());
                showSystemTrayMessage("Flynote", QString("%1 left").arg(user), 2000);
                deleteUser(user, sender);
            } else if (datagram_note.startsWith("note:")){
                qDebug() << "Receive" << datagram_note;

                // Create the note from the data
                QJsonDocument note_doc = QJsonDocument::fromJson(datagram_note.remove(0, QString("note:").length()));
                QJsonObject json_note = note_doc.object();
                notelistModel->insertJsonNote(notelistModel->rowCount(), json_note);

                showSystemTrayMessage("Flynote", "You receive a note", 5000);
            }
        }
    }
}

void MainWindow::systemTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick){
        show();
    }
}

#ifdef _WIN32
void MainWindow::changeWindowsNetwork(const QString &network)
{
    sendedByMe = 2;
    udpSender->writeDatagram(QString("user_left:%1").arg(QHostInfo::localHostName()).toUtf8(), broadcastAddress, broadcast_port);
    udpSender->waitForBytesWritten(1000);
    // Notify the new network
    broadcastAddress = QHostAddress(network);
    udpSender->writeDatagram(QString("user_join:%1").arg(QHostInfo::localHostName()).toUtf8(), broadcastAddress, broadcast_port);
    udpSender->waitForBytesWritten(1000);
}
#endif

