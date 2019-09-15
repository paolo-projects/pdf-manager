#include "pdfsinglepagespecificator.h"

PdfSinglePageSpecificator::PdfSinglePageSpecificator(QString documentPath, QString descriptor, PdfUtil* doc)
    :   docPath(documentPath), doc(doc)
{
    page = descriptor.toInt()-1;
}

PdfSinglePageSpecificator::PdfSinglePageSpecificator(QString documentPath, int pageNum, PdfUtil* doc)
    :
      page(pageNum), docPath(documentPath), doc(doc)
{
}

PdfSinglePageSpecificator::~PdfSinglePageSpecificator()
{

}

QList<int> PdfSinglePageSpecificator::getAllPages() const
{
    return QList<int>({page});
}

QString PdfSinglePageSpecificator::getDisplayText() const
{
    return QString::number(page+1);
}

QString PdfSinglePageSpecificator::getDocumentPath() const
{
    return docPath;
}

PdfUtil *PdfSinglePageSpecificator::getDocument() const
{
    return doc;
}
