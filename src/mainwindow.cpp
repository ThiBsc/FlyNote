#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , fnote(new FlyNote("My note", "Hello world!"))
    , vLayout(new QVBoxLayout(centralWidget()))
    , noteListView(new QListView(this))
    , notelistModel(NoteListModel::getInstance())
{
    ui->setupUi(this);

    noteListView->setModel(notelistModel);

    vLayout->setContentsMargins(QMargins());
    vLayout->addWidget(noteListView);
    centralWidget()->setLayout(vLayout);

    fnote->show();
    setWindowTitle("FlyNote");
    setWindowIcon(QIcon("://icons/note.png"));
    resize(300, 350);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete noteListView;
    delete vLayout;
}
