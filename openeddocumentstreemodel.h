#ifndef OPENEDDOCUMENTSTREEMODEL_H
#define OPENEDDOCUMENTSTREEMODEL_H

#include <QStandardItemModel>
#include <QMimeData>

#include "pdfutil.h"
#include "treeitem.h"
#include "progsettings.h"

#include "pdfpagerangespecificator.h"
#include "pdfpagecontinuousintervalspecificator.h"
#include "pdfsinglepagespecificator.h"

class OpenedDocumentsTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit OpenedDocumentsTreeModel(QList<PdfUtil*>& openedDocuments, QObject *parent = nullptr);
    ~OpenedDocumentsTreeModel() override;
/*
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;*/
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    //bool insertRows(int row, int count, const QModelIndex &parent) override;

    //void addDocument(PdfUtil* doc);
    //void removeDocument(int index);
private:
    QList<PdfUtil*>& openedDocuments;
    TreeItem *rootItem;
    const QStringList defMimeTypes = QStringList{SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE};
    // QAbstractItemModel interface
public:
    QStringList mimeTypes() const override;
};

#endif // OPENEDDOCUMENTSTREEMODEL_H
