#ifndef PDFMULTIPLEPAGESSPECIFICATOR_H
#define PDFMULTIPLEPAGESSPECIFICATOR_H

#include "pdfpagerangespecificator.h"

#include <QString>
#include <QList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

class PdfMultiplePagesSpecificator : public PdfPageRangeSpecificator
{
public:
    PdfMultiplePagesSpecificator(QList<int> pages);
    PdfMultiplePagesSpecificator(QString descriptor);

    ~PdfMultiplePagesSpecificator() override;

    QList<int> getAllPages() override;
    QString getDisplayText() override;

private:
    QList<int> pages;
    QRegularExpression regxp = QRegularExpression("(\\d+),?");
};

#endif // PDFMULTIPLEPAGESSPECIFICATOR_H
