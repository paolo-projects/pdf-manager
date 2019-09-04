#ifndef PDFSINGLEPAGESPECIFICATOR_H
#define PDFSINGLEPAGESPECIFICATOR_H

#include <QString>
#include <QList>

#include "pdfpagerangespecificator.h"

class PdfSinglePageSpecificator : public PdfPageRangeSpecificator
{
public:
    PdfSinglePageSpecificator(QString descriptor);
    PdfSinglePageSpecificator(int pageNum);
    ~PdfSinglePageSpecificator() override;

    QList<int> getAllPages() override;
    QString getDisplayText() override;

private:
    int page;
};

#endif // PDFSINGLEPAGESPECIFICATOR_H
