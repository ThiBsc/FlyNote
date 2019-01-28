#include "notelistmodel.h"

#include "flynote.h"

#include <sstream>

NoteListModel *NoteListModel::instance = nullptr;

NoteListModel::NoteListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

NoteListModel::~NoteListModel()
{
    delete instance;
}

NoteListModel *NoteListModel::getInstance()
{
    if (!instance){
        instance = new NoteListModel();
    }
    return instance;
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
    return (parent.isValid() ? 0 : noteArray.size());
}

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (index.isValid()){
        if (role == Qt::DisplayRole){
            ret = noteArray.at(index.row()).toObject().value("title");
        } else if (role == Qt::BackgroundRole){
            QColor color(noteArray.at(index.row()).toObject().value("color").toString());

            QLinearGradient grad(QPointF(0, 0), QPointF(1, 0));
            grad.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
            grad.setColorAt(1, color);
            //grad.setColorAt(0.8, color.lighter(110));
            //grad.setColorAt(0.55, color.lighter(/*150*/));
            grad.setColorAt(0.5, Qt::white);
            grad.setColorAt(0, Qt::white);

            ret = QBrush(grad);
        } else if (role == Qt::SizeHintRole){
            ret = QSize(-1, 40);
        } else if (role == Qt::FontRole){
            QFont font("Courier");
            ret = font;
        } else {
            // do nothing;
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
    const void *address = static_cast<const void*>(note);
    std::stringstream ss;
    ss << address;
    QJsonObject jsonNote;
    jsonNote.insert("title", note->getTitle());
    jsonNote.insert("color", note->getColor().name());
    jsonNote.insert("address", QString(ss.str().c_str()));
    noteArray.push_back(jsonNote);
    endInsertRows();
}

void NoteListModel::removeNote(int row)
{
    beginRemoveRows(index(row).parent(), row, row);
    noteArray.removeAt(row);
    endRemoveRows();
}

bool NoteListModel::removeNote(FlyNote *note)
{
    bool removed = false;
    int position = notePosition(note);
    if (position != -1){
        removeNote(position);
        removed = true;
    }
    return removed;
}

int NoteListModel::notePosition(FlyNote *note)
{
    int position = -1;
    QJsonArray::iterator res = std::find_if(noteArray.begin(), noteArray.end(), [note](const QJsonValue &jval){
        QJsonObject jobj = jval.toObject();
        const void *address = static_cast<const void*>(note);
        std::stringstream ss;
        ss << address;
        return jobj.value("address").toString() == QString(ss.str().c_str());
    });
    if (res != noteArray.end()){
        position = std::distance(noteArray.begin(), res);
    }
    return position;
}

void NoteListModel::updateNote(FlyNote *note)
{
    int position = notePosition(note);
    if (position != -1){
        QJsonObject jsonNote = noteArray.at(position).toObject();
        jsonNote["title"] = note->getTitle();
        jsonNote["color"] = note->getColor().name();
        noteArray.replace(position, jsonNote);
        emit dataChanged(index(position), index(position), {Qt::DisplayRole, Qt::BackgroundRole});
    }
}

