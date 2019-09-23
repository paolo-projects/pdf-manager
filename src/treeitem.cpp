#include "treeitem.h"

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parentItem, PdfUtil* doc)
    : m_itemData(data), m_parentItem(parentItem), doc(doc)
{

}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *child)
{
    m_childItems.append(child);
}

bool TreeItem::insertEmptyChildren(int row, int count, TreeItem* parent)
{
    if(row >= 0 && row <= m_childItems.count())
    {
        for(int i = row; i < row + count; i++)
        {
            m_childItems.insert(row, new TreeItem(QVector<QVariant>(), parent));
        }
        return true;
    }
    return false;
}

TreeItem *TreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        m_childItems[row] = new TreeItem(QVector<QVariant>{}, this);
    return m_childItems.at(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
            return QVariant();
    return m_itemData.at(column);
}

bool TreeItem::setData(int column, QVariant data)
{
    if (column < 0 || column >= m_itemData.size())
            return false;
    m_itemData[column] = data;
    return true;
}

int TreeItem::row() const
{
    if (m_parentItem)
            return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

void TreeItem::removeChild(int index)
{
    m_childItems.removeAt(index);
}

PdfUtil *TreeItem::GetDocument() const
{
    return doc;
}
