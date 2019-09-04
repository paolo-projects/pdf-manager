#include "pdfmultiplepagesspecificator.h"

PdfMultiplePagesSpecificator::PdfMultiplePagesSpecificator(QList<int> pages)
    : pages(pages)
{

}

/**
 * @brief PdfMultiplePagesSpecificator::PdfMultiplePagesSpecificator
 * @param descriptor A comma separated list of pages e.g. 1,2,3,44,55,66,777
 */
PdfMultiplePagesSpecificator::PdfMultiplePagesSpecificator(QString descriptor)
{
    QRegularExpressionMatchIterator rMatchIt = regxp.globalMatch(descriptor);

    while(rMatchIt.hasNext())
    {
        QRegularExpressionMatch match = rMatchIt.next();
        if(match.isValid())
            pages << (match.captured(1).toInt()-1);
    }
}

PdfMultiplePagesSpecificator::~PdfMultiplePagesSpecificator()
{

}

QList<int> PdfMultiplePagesSpecificator::getAllPages()
{
    return pages;
}

QString PdfMultiplePagesSpecificator::getDisplayText()
{
    QString text = "";

    for (int i = 0; i < pages.length(); i++)
    {
        if(i>0)
            text += ",";

        text += QString::number(pages.at(i)+1);
    }

    return text;
}
