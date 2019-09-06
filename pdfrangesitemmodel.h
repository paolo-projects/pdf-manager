#ifndef PDFRANGESITEMMODEL_H
#define PDFRANGESITEMMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QModelIndex>
#include <QMimeData>
#include <QtDebug>

#include "pdfpagerangespecificator.h"
#include "pdfpagecontinuousintervalspecificator.h"
#include "pdfsinglepagespecificator.h"
#include "progsettings.h"

class PdfRangesItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    PdfRangesItemModel(QList<PdfPageRangeSpecificator*>& itemsList, QObject* parent = nullptr);
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    QMimeData* mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    QList<QString> decodeData(QByteArray& byteArray);
private:
    QList<PdfPageRangeSpecificator*>& items;
    const QStringList supportedMimeTypes = QStringList{SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE};
    const QFlags<Qt::ItemFlag> defaultFlags = QFlags<Qt::ItemFlag>{Qt::ItemIsSelectable, Qt::ItemIsEditable, Qt::ItemIsDragEnabled, Qt::ItemIsDropEnabled, Qt::ItemIsEnabled, Qt::ItemNeverHasChildren};
    const QFlags<Qt::ItemFlag> invalidItemFlags = QFlags<Qt::ItemFlag>{Qt::ItemIsDropEnabled, Qt::ItemIsEnabled};
};

#endif // PDFRANGESITEMMODEL_H
