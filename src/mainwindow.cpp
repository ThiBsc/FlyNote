#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , vLayout(new QVBoxLayout(centralWidget()))
    , noteListView(new QListView(this))
    , notelistModel(NoteListModel::getInstance())
{
    ui->setupUi(this);

    noteListView->setModel(notelistModel);

    vLayout->setContentsMargins(QMargins());
    vLayout->addWidget(noteListView);
    centralWidget()->setLayout(vLayout);

    actAddNote = ui->mainToolBar->addAction(QIcon("://icons/add"), "New note", notelistModel, SLOT(addNote()));
    actEditNote = ui->mainToolBar->addAction(QIcon("://icons/edit"), "Edit note", this, SLOT(editNote()));

    setWindowTitle("FlyNote");
    setWindowIcon(QIcon("://icons/note.png"));
    resize(300, 350);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete noteListView;
    delete actAddNote;
    delete actEditNote;
    delete vLayout;
}

void MainWindow::editNote()
{
    notelistModel->editNote(noteListView->currentIndex());
}
