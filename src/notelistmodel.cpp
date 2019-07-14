#include "notelistmodel.h"
#include "flynote.h"
#include "colorpicker.h"

#include <QFile>
#include <QJsonDocument>
#include <QIcon>
#include <QDir>

#include <sstream>

NoteListModel *NoteListModel::instance = new NoteListModel(nullptr);

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
    return (parent.isValid() ? 0 : noteArray.size());
}

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (index.isValid()){
        if (role == Qt::DisplayRole){
            ret = noteArray.at(index.row()).toObject().value("title");
        } else if (role == Qt::ToolTipRole){
            QString content = noteArray.at(index.row()).toObject().value("content").toString();
            if (content.length() > 40){
                content = content.left(37).leftJustified(40, '.');
            }
            ret = QString("<b>%1</b><br>%2").arg(data(index, Qt::DisplayRole).toString(), content);
        } else if (role == Qt::EditRole){
            ret = data(index, Qt::DisplayRole);
        } else if (role == Qt::DecorationRole){
            if (noteArray.at(index.row()).toObject().contains("received")){
                ret = QIcon(QString("://icons/%1").arg(noteArray.at(index.row()).toObject().value("received").toString()));
            }
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
        } else if (role == Qt::ForegroundRole){
            ret = (noteArray.at(index.row()).toObject().contains("address") ? QColor(Qt::black) : QColor(Qt::gray));
        } else {
            // do nothing;
        }
    }
    return ret;
}

bool NoteListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = false;
    if (data(index, role) != value && role == Qt::EditRole) {
        QJsonObject jsonNote = noteArray.at(index.row()).toObject();
        if (jsonNote.contains("address")){
            // In 64b, we need toULong conversion
            quintptr ptr = jsonNote.value("address").toString().toULong(nullptr, 16);
            auto fnote = reinterpret_cast<FlyNote*>(ptr);
            fnote->setTitle(value.toString());
            jsonNote["title"] = fnote->getTitle();
        } else {
            jsonNote["title"] = value.toString();
        }
        noteArray.replace(index.row(), jsonNote);
        emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole);
        ret = true;
    }
    return ret;
}

Qt::ItemFlags NoteListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags iflags;
    if (index.isValid()){
        iflags = (Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
    } else {
        iflags = Qt::NoItemFlags;
    }
    return iflags;
}

void NoteListModel::insertJsonNote(int row, QJsonObject note)
{
    beginInsertRows(index(row).parent(), row, row);
    noteArray.push_back(note);
    endInsertRows();
}

void NoteListModel::insertNote(int row, FlyNote *note)
{
    beginInsertRows(index(row).parent(), row, row);
    const void *address = static_cast<const void*>(note);
    std::stringstream ss;
    ss << address;
    QJsonObject jsonNote;
    jsonNote.insert("title", note->getTitle());
    jsonNote.insert("content", note->getContent());
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

bool NoteListModel::disableNote(FlyNote *note)
{
    bool disabled = false;
    int position = notePosition(note);
    if (position != -1){
        QJsonObject jsonNote = noteArray.at(position).toObject();
        jsonNote.remove("address");
        noteArray.replace(position, jsonNote);
        disabled = true;
        emit dataChanged(index(position), index(position), {Qt::ForegroundRole});
    }
    return disabled;
}

int NoteListModel::notePosition(FlyNote *note) const
{
    bool finded = false;
    int position = 0;
    const void *address = static_cast<const void*>(note);
    std::stringstream ss;
    ss << address;
    while (!finded && position < noteArray.size()){
        QJsonObject jobj = noteArray.at(position++).toObject();
        finded = jobj.value("address").toString() == QString(ss.str().c_str());
    }
    return (finded ? position-1 : -1);
}

QJsonObject NoteListModel::jsonNote(int index) const
{
    return noteArray.at(index).toObject();
}

void NoteListModel::saveNotes(bool leaveprogram)
{
    if (leaveprogram){
        // When leave, remove address and close opened notes
        for (QJsonValueRef val : noteArray){
            QJsonObject note = val.toObject();
            if (note.contains("address")){
                quintptr ptr = note.value("address").toString().toULong(nullptr, 16);
                delete reinterpret_cast<FlyNote*>(ptr);
                note.remove("address");
                val = note;
            }
        }
    }
    QJsonDocument saveDoc(noteArray);
    QString path = QString("%1/%2").arg(QDir::homePath(), ".flynote");
    QDir dir(path);
    if (!dir.exists()){
        dir.mkpath(path);
    }
    QFile saveFile(QString("%1/flynote.json").arg(path));
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    } else {
        saveFile.write(saveDoc.toJson());
        saveFile.close();
    }
}

void NoteListModel::readNotes()
{
    QString file_path = QString("%1/%2/flynote.json").arg(QDir::homePath(), ".flynote");
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
    } else {
        QByteArray notesData = file.readAll();
        file.close();
        QJsonDocument loadNotes(QJsonDocument::fromJson(notesData));
        beginResetModel();
        noteArray = loadNotes.array();
        endResetModel();
    }
}

void NoteListModel::addNote()
{
    beginInsertRows(index(rowCount()).parent(), rowCount(), rowCount());
    QJsonObject jsonNote;
    jsonNote.insert("title", QString("New note #%1").arg(rowCount()+1));
    jsonNote.insert("content", QString(""));
    jsonNote.insert("color", ColorPicker::getRandomColor().name());
    noteArray.push_back(jsonNote);
    endInsertRows();
}

void NoteListModel::editNote(const QModelIndex &noteindex)
{
    if (noteindex.isValid()){
        QJsonObject jsonNote = noteArray.at(noteindex.row()).toObject();
        if (jsonNote.contains("address")){
            // do nothing;
            //quintptr ptr = jsonNote.value("address").toString().toULong(nullptr, 16);
            //reinterpret_cast<FlyNote*>(ptr)->setFocus(Qt::FocusReason::MouseFocusReason);
        } else {
            FlyNote *newNote = new FlyNote(QColor(jsonNote.value("color").toString()), jsonNote.value("title").toString(), jsonNote.value("content").toString());
            newNote->show();
            const void *address = static_cast<const void*>(newNote);
            std::stringstream ss;
            ss << address;
            jsonNote.insert("address", QString(ss.str().c_str()));
            noteArray.replace(noteindex.row(), jsonNote);
            emit dataChanged(index(rowCount()-1), index(rowCount()-1), {Qt::ForegroundRole});
        }
    }
}
#include <QDebug>
void NoteListModel::deleteNote(const QModelIndex &noteindex)
{
    if (noteindex.isValid()){
        QJsonObject jsonNote = noteArray.at(noteindex.row()).toObject();
        if (jsonNote.contains("address")){
            quintptr ptr = jsonNote.value("address").toString().toULong(nullptr, 16);
            delete reinterpret_cast<FlyNote*>(ptr);
        }
        removeNote(noteindex.row());
    }
}

void NoteListModel::updateNote(FlyNote *note)
{
    int position = notePosition(note);
    if (position != -1){
        QJsonObject jsonNote = noteArray.at(position).toObject();
        jsonNote["title"] = note->getTitle();
        jsonNote["content"] = note->getContent();
        jsonNote["color"] = note->getColor().name();
        noteArray.replace(position, jsonNote);
        emit dataChanged(index(position), index(position), {Qt::DisplayRole, Qt::BackgroundRole});
    }
}

