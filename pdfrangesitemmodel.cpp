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
        return items.at(index.row())->getDisplayText();
    else if (role == Qt::EditRole)
        return QVariant::fromValue<PdfPageRangeSpecificator*>(items.at(index.row()));
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
        items.removeAt(position);
    }

    endRemoveRows();
    return true;
}

bool PdfRangesItemModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild);
    for(int i = 0; i<count; ++i) {
        items.insert(destinationChild + i, items[sourceRow]);
        int removeIndex = destinationChild > sourceRow ? sourceRow : sourceRow+1;
        items.removeAt(removeIndex);
    }
    endMoveRows();
    return true;
}

bool PdfRangesItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    qDebug() << "dropMimeData\n";
    int rowPosition = parent.isValid() ? row+1 : row;
    rowPosition = rowPosition == -1 ? rowCount() : rowPosition;
    if(data->hasFormat("application/x-qabstractitemmodeldatalist"))
    {
        QByteArray bytearray = data->data("application/x-qabstractitemmodeldatalist");
        auto data_items = decodeData(bytearray);
        qDebug() << "Data decoded: " << data_items << "\n";

        if(data_items.length() > 1)
        {
            PdfPageRangeSpecificator* newItem = new PdfPageContinuousIntervalSpecificator(data_items.first().toInt()-1, data_items.last().toInt()-1);
            if(insertRow(rowPosition))
            {
                auto t_index = index(rowPosition);
                setData(t_index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
            }
        } else {
            PdfPageRangeSpecificator* newItem = new PdfSinglePageSpecificator(data_items.first().toInt()-1);
            if(insertRow(rowPosition))
            {
                auto t_index = index(rowCount()-1);
                setData(t_index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
            }
        }
        return true;
    } else if (data->hasFormat(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE))
    {
        //return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
        QList<intptr_t> list;
        QByteArray b_data = data->data(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE);
        QDataStream p_data(&b_data, QIODevice::ReadOnly);
        int startingRow;
        p_data >> startingRow;
        if(startingRow != rowPosition)
        {
            intptr_t p;
            while(!p_data.atEnd())
            {
                p_data.readRawData(reinterpret_cast<char*>(&p), sizeof(intptr_t));
                list << p;
            }

            removeRows(startingRow, list.size());
            if(rowPosition > startingRow)
                rowPosition -= list.size();

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

bool PdfRangesItemModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    qDebug() << "canDropMimeData, row:" << row << "\n";

    if(data->hasFormat(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE) || data->hasFormat("application/x-qabstractitemmodeldatalist"))
        return true;

    qDebug() << "Falling back to parent method.\n";
    return QAbstractItemModel::canDropMimeData(data, action, row, column, parent);
}

QMimeData *PdfRangesItemModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug() << "mimeData\n";
    if(indexes.length() == 0)
        return 0;

    QModelIndexList indexes_s(indexes);
    std::sort(indexes_s.begin(), indexes_s.end(), std::less<QModelIndex>());

    QMimeData* mimeData = new QMimeData;
    QByteArray pointersData;
    QDataStream pointersStream(&pointersData, QIODevice::WriteOnly);

    pointersStream << indexes_s.at(0).row();

    for(const QModelIndex& index : indexes_s)
    {
        intptr_t p = reinterpret_cast<intptr_t>(items.at(index.row()));
        pointersStream.writeRawData(reinterpret_cast<char*>(&p), sizeof(intptr_t));
    }

    mimeData->setData(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE, pointersData);

    return mimeData;
}

QStringList PdfRangesItemModel::mimeTypes() const
{
    qDebug() << "mimeTypes\n";
    return supportedMimeTypes;
}

Qt::DropActions PdfRangesItemModel::supportedDragActions() const
{
    qDebug() << "supportedDragActions\n";
    return QFlags<Qt::DropAction>{Qt::CopyAction, Qt::MoveAction};
}

Qt::DropActions PdfRangesItemModel::supportedDropActions() const
{
    qDebug() << "supportedDropActions\n";
    return QFlags<Qt::DropAction>{Qt::CopyAction, Qt::MoveAction};
}

QList<QString> PdfRangesItemModel::decodeData(QByteArray &byteArray)
{
    QList<QString> items;
    QDataStream ds(&byteArray, QIODevice::ReadOnly);
    while(!ds.atEnd())
    {
        /*int row;
        int column;
        int map_items;

        ds >> row;
        ds >> column;
        ds >> map_items;

        for(int i = 0; i < map_items; i++) {
            int key;
            ds >> key;
            QVariant val;
            ds >> val;
            items << qvariant_cast<QString>(val);
        }*/
        int row, col;
        QMap<int,  QVariant> roleDataMap;
        ds >> row >> col >> roleDataMap;

        items.append(roleDataMap.value(Qt::DisplayRole).toString());
    }
    return items;
}
