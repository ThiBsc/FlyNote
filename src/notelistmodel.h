#ifndef NOTELISTMODEL_H
#define NOTELISTMODEL_H

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonObject>

class FlyNote;

class NoteListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit NoteListModel(QObject *parent = nullptr);
    ~NoteListModel() override;
    static NoteListModel *instance;
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    // Manage data:
    void insertNote(int row, FlyNote *note);
    void removeNote(int row);
    bool removeNote(FlyNote *note);
    bool disableNote(FlyNote *note);
    int notePosition(FlyNote *note);
    void saveNotes();
    void readNotes();

public slots:
    void addNote();
    void editNote(const QModelIndex &noteindex);
    void deleteNote(const QModelIndex &noteindex);
    void updateNote(FlyNote *note);

private:
    QJsonArray noteArray;


};

#endif // NOTELISTMODEL_H
