#include "pdfmultiplepagesspecificator.h"

PdfMultiplePagesSpecificator::PdfMultiplePagesSpecificator(QString documentPath, QList<int> pages, PdfUtil* doc)
    : pages(pages), docPath(documentPath), doc(doc)
{

}

/**
 * @brief PdfMultiplePagesSpecificator::PdfMultiplePagesSpecificator
 * @param descriptor A comma separated list of pages e.g. 1,2,3,44,55,66,777
 */
PdfMultiplePagesSpecificator::PdfMultiplePagesSpecificator(QString documentPath, QString descriptor, PdfUtil* doc)
    : docPath(documentPath), doc(doc)
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

QList<int> PdfMultiplePagesSpecificator::getAllPages() const
{
    return pages;
}

QString PdfMultiplePagesSpecificator::getDisplayText() const
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

QString PdfMultiplePagesSpecificator::getDocumentPath() const
{
    return docPath;
}

PdfUtil *PdfMultiplePagesSpecificator::getDocument() const
{
    return doc;
}

bool PdfMultiplePagesSpecificator::isImage() const
{
    return false;
}
