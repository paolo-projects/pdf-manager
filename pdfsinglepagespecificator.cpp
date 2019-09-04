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

QList<int> PdfSinglePageSpecificator::getAllPages()
{
    return QList<int>({page});
}

QString PdfSinglePageSpecificator::getDisplayText()
{
    return QString::number(page+1);
}
