#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>

#ifdef _WIN32
// Manage the QHostAddress::Broadcast problem on windows
#include <QNetworkInterface>
#include <QNetworkConfigurationManager>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , vLayout(new QVBoxLayout(centralWidget()))
    , noteListView(new QListView(this))
    , notelistModel(NoteListModel::instance)
    , udpSender(new QUdpSocket(this))
    , udpReceiver(new QUdpSocket(this))
    , sendedByMe(false)
#ifdef _WIN32
    , networkCbx(new QComboBox(this))
#endif
{
    ui->setupUi(this);

    noteListView->setModel(notelistModel);

    vLayout->setContentsMargins(QMargins());
    vLayout->addWidget(noteListView);
    centralWidget()->setLayout(vLayout);

    ui->mainToolBar->addAction(QIcon("://icons/add"), "New note", notelistModel, SLOT(addNote()));
    ui->mainToolBar->addAction(QIcon("://icons/delete"), "Delete note", this, SLOT(deleteNote()));
    ui->mainToolBar->addAction(QIcon("://icons/edit"), "Edit note", this, SLOT(editNote()));
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon("://icons/send"), "Send", this, SLOT(sendBroadcastNote()));

    /** Network management */
    // Test broadcast with 'nc -u 192.168.0.255 45454
#ifdef _WIN32
    // Add in a combobox all real broadcast address to avoid the use of QHostAddress::Broadcast in Windows
    ui->mainToolBar->addWidget(networkCbx);
    networkCbx->setEditable(true);
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
#endif
    udpReceiver->bind(broadcast_port, QUdpSocket::ShareAddress);
    connect(udpReceiver, SIGNAL(readyRead()), this, SLOT(receiveBroadcastNote()));

    setWindowTitle("FlyNote");
    setWindowIcon(QIcon("://icons/note.png"));
    resize(300, 350);
}

MainWindow::~MainWindow()
{
    delete ui;
#ifdef _WIN32
    delete networkCbx;
#endif
    delete udpSender;
    delete udpReceiver;
    delete noteListView;
    delete vLayout;
}

void MainWindow::editNote()
{
    notelistModel->editNote(noteListView->currentIndex());
}

void MainWindow::deleteNote()
{
    notelistModel->deleteNote(noteListView->currentIndex());
}

/**
 * @brief MainWindow::sendBroadcastNote
 * https://doc.qt.io/qt-5/qtnetwork-broadcastsender-example.html
 */
void MainWindow::sendBroadcastNote()
{
    QModelIndex current_note = noteListView->currentIndex();
    if (current_note.isValid()){
        // get the note to broadcast
        QJsonObject json_note = notelistModel->jsonNote(current_note.row());
        json_note.remove("address");
        QJsonDocument doc(json_note);

        QByteArray datagram_note = doc.toJson();
        qDebug() << "Send" << datagram_note;
        sendedByMe = true;
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
void MainWindow::receiveBroadcastNote()
{
    QByteArray datagram_note;
    while (udpReceiver->hasPendingDatagrams()){
        datagram_note.resize(int(udpReceiver->pendingDatagramSize()));
        udpReceiver->readDatagram(datagram_note.data(), datagram_note.size());
    }
    if (sendedByMe){
        sendedByMe = false;
    } else {
        qDebug() << "Receive" << datagram_note;

        // Create the note from the data
        QJsonDocument note_doc = QJsonDocument::fromJson(datagram_note);
        QJsonObject json_note = note_doc.object();
        json_note.insert("received", "lan");
        notelistModel->insertJsonNote(notelistModel->rowCount(), json_note);
    }
}
