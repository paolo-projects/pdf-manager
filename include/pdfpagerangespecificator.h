#ifndef PDFPAGERANGESPECIFICATOR_H
#define PDFPAGERANGESPECIFICATOR_H

#include <QList>

#include "pdfutil.h"

class PdfPageRangeSpecificator
{
public:
    virtual ~PdfPageRangeSpecificator() = default;
    virtual QList<int> getAllPages() const = 0;
    virtual QString getDisplayText() const = 0;
    operator QString() const { return getDisplayText(); }
    operator QList<int>() const { return getAllPages(); }
    virtual QString getDocumentPath() const = 0;
    virtual PdfUtil* getDocument() const = 0;
    virtual bool isImage() const = 0;
};

Q_DECLARE_METATYPE(PdfPageRangeSpecificator*)

#endif // PDFPAGERANGESPECIFICATOR_H
