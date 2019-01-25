#include "notelistmodel.h"

#include "flynote.h"

NoteListModel::NoteListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

NoteListModel::~NoteListModel()
{

}

QVariant NoteListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;
    if (section==0 && orientation==Qt::Horizontal && role==Qt::DisplayRole){
        ret = tr("Flying notes");
    }
    return ret;
}

int NoteListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    return (parent.isValid() ? 0 : 0);
}

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (index.isValid()){
        switch (role) {
        case Qt::DisplayRole:
            break;
        default:
            break;
        }
    }
    return ret;
}

bool NoteListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags NoteListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags iflags;
    if (index.isValid()){
        iflags = (Qt::ItemIsEnabled | Qt::ItemIsEditable);
    } else {
        iflags = Qt::NoItemFlags;
    }
    return iflags;
}

void NoteListModel::insertNote(int row, FlyNote *note)
{
    beginInsertRows(index(row).parent(), row, row);
    endInsertRows();
}

void NoteListModel::removeNote(int row)
{
    beginRemoveRows(index(row).parent(), row, row);
    endRemoveRows();
}

