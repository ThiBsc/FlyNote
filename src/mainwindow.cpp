#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , vLayout(new QVBoxLayout(centralWidget()))
    , noteListView(new QListView(this))
    , notelistModel(NoteListModel::instance)
{
    ui->setupUi(this);

    noteListView->setModel(notelistModel);

    vLayout->setContentsMargins(QMargins());
    vLayout->addWidget(noteListView);
    centralWidget()->setLayout(vLayout);

    ui->mainToolBar->addAction(QIcon("://icons/add"), "New note", notelistModel, SLOT(addNote()));
    ui->mainToolBar->addAction(QIcon("://icons/delete"), "Delete note", this, SLOT(deleteNote()));
    ui->mainToolBar->addAction(QIcon("://icons/edit"), "Edit note", this, SLOT(editNote()));

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

void MainWindow::editNote()
{
    notelistModel->editNote(noteListView->currentIndex());
}

void MainWindow::deleteNote()
{
    notelistModel->deleteNote(noteListView->currentIndex());
}
