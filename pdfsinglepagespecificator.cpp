#include "pdfsinglepagespecificator.h"

PdfSinglePageSpecificator::PdfSinglePageSpecificator(QString descriptor)
{
    page = descriptor.toInt()-1;
}

PdfSinglePageSpecificator::PdfSinglePageSpecificator(int pageNum)
    :
      page(pageNum)
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
