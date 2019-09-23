#ifndef PDFSINGLEPAGESPECIFICATOR_H
#define PDFSINGLEPAGESPECIFICATOR_H

#include <QString>
#include <QList>

#include "pdfpagerangespecificator.h"

class PdfSinglePageSpecificator : public PdfPageRangeSpecificator
{
public:
    PdfSinglePageSpecificator(QString documentPath, QString descriptor, PdfUtil* doc);
    PdfSinglePageSpecificator(QString documentPath, int pageNum, PdfUtil* doc);
    ~PdfSinglePageSpecificator() override;

    QList<int> getAllPages() const override;
    QString getDisplayText() const override;

    QString getDocumentPath() const override;
    PdfUtil* getDocument() const override;

private:
    int page;

    QString docPath;
    PdfUtil* doc;
};

#endif // PDFSINGLEPAGESPECIFICATOR_H
