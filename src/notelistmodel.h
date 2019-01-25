#ifndef NOTELISTMODEL_H
#define NOTELISTMODEL_H

#include <QAbstractListModel>

class FlyNote;

class NoteListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit NoteListModel(QObject *parent = nullptr);
    ~NoteListModel() override;

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

private:

};

#endif // NOTELISTMODEL_H
