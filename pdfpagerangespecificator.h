#ifndef PDFPAGERANGESPECIFICATOR_H
#define PDFPAGERANGESPECIFICATOR_H

#include <QList>

class PdfPageRangeSpecificator
{
public:
    virtual ~PdfPageRangeSpecificator() = default;
    virtual QList<int> getAllPages() = 0;
    virtual QString getDisplayText() = 0;
};

#endif // PDFPAGERANGESPECIFICATOR_H
