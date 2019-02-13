#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    NoteListModel::instance->readNotes();

    MainWindow w;
    w.show();

    int ret = a.exec();
    NoteListModel::instance->saveNotes();
    delete NoteListModel::instance;

    return ret;
}
