#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QVariant>

#include "pdfutil.h"

class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant> &data, TreeItem *parentItem = nullptr, PdfUtil* doc = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool setData(int column, QVariant data);
    int row() const;
    TreeItem *parentItem();
    void removeChild(int index);
    PdfUtil* GetDocument() const;

    bool insertEmptyChildren(int row, int count, TreeItem *parent);
private:
    QVector<TreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    TreeItem *m_parentItem;
    PdfUtil* doc;
};
#endif // TREEITEM_H
