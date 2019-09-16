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
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
private:
    QList<PdfUtil*>& openedDocuments;
    TreeItem *rootItem;
    const QStringList defMimeTypes = QStringList{SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE};
    // QAbstractItemModel interface
public:
    QStringList mimeTypes() const override;
};

#endif // OPENEDDOCUMENTSTREEMODEL_H
