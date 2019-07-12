#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // To let the systemTrayIcon manage this
    a.setQuitOnLastWindowClosed(false);

    NoteListModel::instance->readNotes();
    MainWindow w;
    w.show();

    int ret = a.exec();
    NoteListModel::instance->saveNotes(true);
    delete NoteListModel::instance;

    return ret;
}
