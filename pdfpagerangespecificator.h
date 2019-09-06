#ifndef PDFPAGERANGESPECIFICATOR_H
#define PDFPAGERANGESPECIFICATOR_H

#include <QList>

class PdfPageRangeSpecificator
{
public:
    virtual ~PdfPageRangeSpecificator() = default;
    virtual QList<int> getAllPages() const = 0;
    virtual QString getDisplayText() const = 0;
    operator QString() const { return getDisplayText(); }
    operator QList<int>() const { return getAllPages(); }
};

Q_DECLARE_METATYPE(PdfPageRangeSpecificator*)

#endif // PDFPAGERANGESPECIFICATOR_H
