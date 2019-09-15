#include "openeddocumentstreemodel.h"

OpenedDocumentsTreeModel::OpenedDocumentsTreeModel(QList<PdfUtil *> &openedDocuments, QObject *parent)
    : QStandardItemModel(parent), openedDocuments(openedDocuments)
{
    //rootItem = new TreeItem(QVector<QVariant>(), nullptr, nullptr);
}

OpenedDocumentsTreeModel::~OpenedDocumentsTreeModel()
{

}
/*
QVariant OpenedDocumentsTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
            return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags OpenedDocumentsTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return Qt::NoItemFlags;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if(item->childCount()>0)
        return QFlags<Qt::ItemFlag>{Qt::ItemIsEnabled};
    else
        return QFlags<Qt::ItemFlag>{Qt::ItemIsEnabled, Qt::ItemIsSelectable, Qt::ItemIsDragEnabled};
}

QVariant OpenedDocumentsTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return rootItem->data(section);

    return QVariant();
}

QModelIndex OpenedDocumentsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex OpenedDocumentsTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
            return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int OpenedDocumentsTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int OpenedDocumentsTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
            return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

bool OpenedDocumentsTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    return childItem->setData(index.column(), value);
}*/

QMimeData *OpenedDocumentsTreeModel::mimeData(const QModelIndexList &indexes) const
{
    if(indexes.first().parent() != indexes.last().parent())
        return nullptr;

    PdfPageRangeSpecificator* range;


    PdfUtil* pdfDoc = openedDocuments.at(indexes.first().parent().row());

    if(indexes.size() > 1)
        range = new PdfPageContinuousIntervalSpecificator(pdfDoc->GetDocPath(), qvariant_cast<QString>(indexes.first().data()).toInt()-1, qvariant_cast<QString>(indexes.last().data()).toInt() - 1, pdfDoc);
    else
        range = new PdfSinglePageSpecificator(pdfDoc->GetDocPath(), qvariant_cast<QString>(indexes.first().data()).toInt()-1, pdfDoc);

    //DocumentRangeData dranged(pdfDoc, range);
    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);

    dataStream << int(-1);

    intptr_t pointer = reinterpret_cast<intptr_t>(range);
    dataStream << pointer;

    QMimeData* mimeData = new QMimeData();
    mimeData->setData(defMimeTypes[0], data);
    return mimeData;
}
/*

bool OpenedDocumentsTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
    if(parentItem == nullptr)
        parentItem = rootItem;
    return parentItem->insertEmptyChildren(row, count, parentItem);
}

void OpenedDocumentsTreeModel::addDocument(PdfUtil *doc)
{
    openedDocuments.append(doc);
    int parentPos = rootItem->childCount();
    if(insertRow(parentPos, QModelIndex()))
    {
        TreeItem* docItem = new TreeItem(QVector<QVariant>{doc->GetDocName()}, rootItem, doc);
        for(int i = 1; i <= doc->GetPageCount(); i++)
        {
            TreeItem* pageChildItem = new TreeItem(QVector<QVariant>{QString::number(i)}, docItem);
            docItem->appendChild(pageChildItem);
        }
        rootItem->appendChild(docItem);
    }
}

void OpenedDocumentsTreeModel::removeDocument(int index)
{
    openedDocuments.removeAt(index);
    rootItem->removeChild(index);
}*/

QStringList OpenedDocumentsTreeModel::mimeTypes() const
{
    return defMimeTypes;
}
