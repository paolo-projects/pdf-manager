#include "pdfrangesitemmodel.h"

PdfRangesItemModel::PdfRangesItemModel(QList<PdfPageRangeSpecificator*>& itemsList, QObject* parent)
    :   QAbstractItemModel(parent),
      items(itemsList)
{
}

QModelIndex PdfRangesItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
            return createIndex(row, column, items.at(row));
    return QModelIndex();
}

QModelIndex PdfRangesItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int PdfRangesItemModel::rowCount(const QModelIndex &parent) const
{
    return items.length();
}

int PdfRangesItemModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant PdfRangesItemModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid | QAbstractItemModel::CheckIndexOption::ParentIsInvalid));

    if (role == Qt::DisplayRole)
        return QString("<b>%1</b><br/><i>%2</i>").arg(items.at(index.row())->getDisplayText(), items.at(index.row())->getDocument()->GetDocName());
    else if (role == Qt::EditRole)
        return QVariant::fromValue<PdfPageRangeSpecificator*>(items.at(index.row()));
    else if (role == Qt::ToolTipRole)
        return QString("%1 (%2)").arg(items.at(index.row())->getDisplayText(), items.at(index.row())->getDocument()->GetDocName());
    else
        return QVariant();
}

bool PdfRangesItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        items.replace(index.row(), qvariant_cast<PdfPageRangeSpecificator*>(value));
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags PdfRangesItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return invalidItemFlags;
        //return Qt::ItemIsEnabled;

    return defaultFlags;
}

/**
 * @brief PdfRangesItemModel::insertRows Inserts an amount of empty (nullptr) rows in the model and in the bound data structure
 * @param position
 * @param rows
 * @param parent
 * @return
 */
bool PdfRangesItemModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    if (parent.isValid() || rows == 0)
        return false;

    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        items.insert(position, nullptr);
    }

    endInsertRows();
    return true;
}

bool PdfRangesItemModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    return false;
}

bool PdfRangesItemModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        auto p = items.at(position);
        items.removeAt(position);
        delete p;
    }

    endRemoveRows();
    return true;
}

bool PdfRangesItemModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{ //TODO: fix move rows
    beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild);

//    QList<int> removeIndexes;

    for(int i = destinationChild; i<(destinationChild+count); ++i) {
        items.insert(i, items[sourceRow + i - destinationChild]);
//        removeIndexes << (destinationChild > sourceRow ? sourceRow : sourceRow+count);
    }

//    for(auto index : removeIndexes)
//        items.removeAt(index);
    int deltaCount = (destinationChild < sourceRow) ? count : 0;
    for(int i = sourceRow + deltaCount; i < (sourceRow + count + deltaCount); i++)
        items.removeAt(i);

    endMoveRows();
    return true;
}

bool PdfRangesItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    int rowPosition = parent.isValid() ? row+1 : row;
    rowPosition = (rowPosition == -1) ? rowCount() : rowPosition;

    if (data->hasFormat(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE))
    {
        QByteArray b_data = data->data(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE);
        QDataStream p_data(&b_data, QIODevice::ReadOnly);
        int startingRow;
        p_data >> startingRow;

        if(startingRow > -1)//action == Qt::MoveAction)
        {
            // Move from the same QListView
            QList<intptr_t> list;
            QByteArray b_data = data->data(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE);
            QDataStream p_data(&b_data, QIODevice::ReadOnly);
            int startingRow;
            p_data >> startingRow;
            if(startingRow != rowPosition)
            {
                while(!p_data.atEnd())
                {
                    intptr_t p;
                    RawPointer r_p;
                    p_data >> r_p;
                    p = r_p.pointer;
                    qDebug() << "Read pointer: " << p << "\n";
                    list << p;
                }
                moveRows(QModelIndex(), startingRow, list.size(), QModelIndex(), rowPosition);
            }
            return true;
        } else if (startingRow == -1)//action == Qt::CopyAction)
        {
            // Copy from QTreeView
            QList<intptr_t> list;

            if(startingRow == -1)
            {
                intptr_t p;
                while(!p_data.atEnd())
                {
                    RawPointer r_p;
                    p_data >> r_p;
                    p = r_p.pointer;
                    list << p;
                }

                insertRows(rowPosition, list.size());
                for(int i = 0; i < list.size(); i++)
                {
                    auto t_item = index(rowPosition + i);
                    setData(t_item, QVariant::fromValue<PdfPageRangeSpecificator*>(reinterpret_cast<PdfPageRangeSpecificator*>(list.at(i))));
                }
            }
            return true;
        } else return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
    }
    else return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
}

bool PdfRangesItemModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if(data->hasFormat(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE))
        return true;

    return QAbstractItemModel::canDropMimeData(data, action, row, column, parent);
}

QMimeData *PdfRangesItemModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug() << "mimeData\n";
    if(indexes.length() == 0)
        return nullptr;

    QModelIndexList indexes_s(indexes);
    std::sort(indexes_s.begin(), indexes_s.end(), std::less<QModelIndex>());

    QMimeData* mimeData = new QMimeData;
    QByteArray pointersData;
    QDataStream pointersStream(&pointersData, QIODevice::WriteOnly);

    pointersStream << indexes_s.at(0).row();

    for(const QModelIndex& index : indexes_s)
    {
        intptr_t p = reinterpret_cast<intptr_t>(items.at(index.row()));
        //pointersStream.writeRawData(reinterpret_cast<char*>(&p), sizeof(intptr_t));
        qDebug() << "Writing pointer: " << p << "\n";
        RawPointer r_p;
        pointersStream << r_p;
        p = r_p.pointer;
    }

    mimeData->setData(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE, pointersData);

    return mimeData;
}

QStringList PdfRangesItemModel::mimeTypes() const
{
    return supportedMimeTypes;
}

Qt::DropActions PdfRangesItemModel::supportedDragActions() const
{
    return supportedDrag_Actions;
}

Qt::DropActions PdfRangesItemModel::supportedDropActions() const
{
    return supportedDrop_Actions;
}

QList<QString> PdfRangesItemModel::decodeData(QByteArray &byteArray)
{
    QList<QString> items;
    QDataStream ds(&byteArray, QIODevice::ReadOnly);
    while(!ds.atEnd())
    {
        int row, col;
        QMap<int,  QVariant> roleDataMap;
        ds >> row >> col >> roleDataMap;

        items.append(roleDataMap.value(Qt::DisplayRole).toString());
    }
    return items;
}

void PdfRangesItemModel::removeRowsOfDocument(PdfUtil *doc)
{
    for(int r = items.length() - 1; r >= 0; r--)
    {
        auto range = items.at(r);
        if(range->getDocument() == doc)
        {
            removeRow(r);
        }
    }
}
