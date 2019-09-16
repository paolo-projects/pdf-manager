#include "openeddocumentstreemodel.h"

OpenedDocumentsTreeModel::OpenedDocumentsTreeModel(QList<PdfUtil *> &openedDocuments, QObject *parent)
    : QStandardItemModel(parent), openedDocuments(openedDocuments)
{
    //rootItem = new TreeItem(QVector<QVariant>(), nullptr, nullptr);
}

OpenedDocumentsTreeModel::~OpenedDocumentsTreeModel()
{

}

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

QStringList OpenedDocumentsTreeModel::mimeTypes() const
{
    return defMimeTypes;
}
