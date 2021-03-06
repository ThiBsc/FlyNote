#include "notelistmodel.h"
#include "flynote.h"
#include "colorpicker.h"

#include <QSaveFile>
#include <QIcon>
#include <QDir>
#include <QMimeData>

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
        QJsonObject obj = noteArray.at(index.row()).toObject();
        if (role == Qt::DisplayRole){
            ret = obj.value("title");
        } else if (role == Qt::ToolTipRole){
            QString content = obj.value("content").toString();
            if (content.length() > 40){
                content = content.left(37).leftJustified(40, '.');
            }
            QString from;
            if (obj.contains("from")){
                from = QString(" (from %1)").arg(obj.value("from").toString());
            }
            ret = QString("<b>%1</b>%2<br>%3").arg(obj.value("title").toString(), from, content);
        } else if (role == Qt::EditRole){
            ret = data(index, Qt::DisplayRole);
        } else if (role == Qt::DecorationRole){
            if (obj.contains("received")){
                ret = QIcon(QString("://icons/%1").arg(obj.value("received").toString()));
            }
        } else if (role == Qt::BackgroundRole){
            QColor color(obj.value("color").toString());

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
            ret = (obj.contains("address") ? QColor(Qt::black) : QColor(Qt::gray));
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
    Qt::ItemFlags iflags = QAbstractListModel::flags(index);
    if (index.isValid()){
        iflags |= (Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    }
    return iflags;
}
#include <QDebug>
bool NoteListModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    bool ret = data->hasFormat("application/flynote_json");
    return ret;
}

bool NoteListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    bool ret = false;
    if (canDropMimeData(data, action, row, column, parent)){
        int destRow;
        if (row != -1){
            destRow = row;
        } else if (parent.isValid()){
            destRow = parent.row();
        } else {
            destRow = rowCount();
        }
        QByteArray encodedNote = data->data("application/flynote_json");
        QByteArray encodedOriginRow = data->data("origin_row");
        QJsonObject jobj = QJsonDocument::fromJson(encodedNote).object();
        // move encodedOriginRow to destRow
        moveRow(encodedOriginRow.toInt(), destRow);
    }
    return ret;
}

QStringList NoteListModel::mimeTypes() const
{
    QStringList mimes;
    mimes << "application/flynote_json" << "origin_row";
    return mimes;
}

QMimeData *NoteListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *ret = nullptr;
    if (indexes.size() == 1){
        QModelIndex mi = indexes.first();
        if (mi.isValid()){
            /** Doesn't work, I don't know why
             * qRegisterMetaType<QJsonValue>("QJsonValue");
             * qRegisterMetaTypeStreamOperators<QJsonValue>("QJsonValue");
             *
             * QDataStream &operator<<(QDataStream &out, const QJsonValue &myObj){ ... }
             * QDataStream &operator>>(QDataStream &in, QJsonValue &myObj){ ... }
             * ret = QAbstractListModel::mimeData(indexes);
             */
            ret = new QMimeData();
            QJsonDocument json_mime(jsonNote(mi.row()));
            ret->setData("application/flynote_json", json_mime.toJson());
            ret->setData("origin_row", QString::number(mi.row()).toUtf8());
        }
    }
    return ret;
}

Qt::DropActions NoteListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void NoteListModel::moveRow(int originRow, int destRow)
{
    beginRemoveRows(QModelIndex(), originRow, originRow);
    QJsonValue v_at = noteArray.takeAt(originRow);
    endRemoveRows();
    beginInsertRows(QModelIndex(), destRow, destRow);
    noteArray.insert(destRow, v_at);
    endInsertRows();
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
    QJsonArray array = noteArray;
    if (leaveprogram){
        // When leave, close opened notes
        for (QJsonValueRef val : array){
            QJsonObject note = val.toObject();
            if (note.contains("address")){
                quintptr ptr = note.value("address").toString().toULong(nullptr, 16);
                delete reinterpret_cast<FlyNote*>(ptr);
                note.remove("address");
                val = note;
            }
        }
    }
    // When save, remove address
    for (QJsonValueRef val : array){
        QJsonObject note = val.toObject();
        if (note.contains("address")){
            note.remove("address");
            val = note;
        }
    }
    QJsonDocument saveDoc(array);
    QString path = QString("%1/%2").arg(QDir::homePath(), ".flynote");
    QDir dir(path);
    if (!dir.exists()){
        dir.mkpath(path);
    }
    QSaveFile saveFile(QString("%1/flynote.json").arg(path));
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    } else {
        saveFile.write(saveDoc.toJson());
        if (saveFile.commit()){
            // do nothing;
        } else {
            qWarning("Fail to save the file.");
        }
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

void NoteListModel::saveOnModelMove()
{
    saveNotes(false);
}

void NoteListModel::saveOnModelMove(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    if (!roles.contains(Qt::ForegroundRole)){
        // Save only if we realy change the data
        saveNotes(false);
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

