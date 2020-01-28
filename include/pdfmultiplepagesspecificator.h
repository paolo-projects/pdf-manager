#ifndef PDFMULTIPLEPAGESSPECIFICATOR_H
#define PDFMULTIPLEPAGESSPECIFICATOR_H

#include "pdfpagerangespecificator.h"

#include <QString>
#include <QList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QPointer>

class PdfMultiplePagesSpecificator : public PdfPageRangeSpecificator
{
public:
    PdfMultiplePagesSpecificator(QString documentPath, QList<int> pages, PdfUtil* doc);
    PdfMultiplePagesSpecificator(QString documentPath, QString descriptor, PdfUtil* doc);

    ~PdfMultiplePagesSpecificator() override;

    QList<int> getAllPages() const override;
    QString getDisplayText() const override;

    QString getDocumentPath() const override;
    PdfUtil* getDocument() const override;
    bool isImage() const override;

private:
    QList<int> pages;
    QRegularExpression regxp = QRegularExpression("(\\d+),?");

    QString docPath;
    QPointer<PdfUtil> doc;
};

#endif // PDFMULTIPLEPAGESSPECIFICATOR_H
