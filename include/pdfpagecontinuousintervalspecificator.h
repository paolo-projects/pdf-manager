#ifndef PDFPAGECONTINUOUSINTERVALSPECIFICATOR_H
#define PDFPAGECONTINUOUSINTERVALSPECIFICATOR_H

#include "pdfpagerangespecificator.h"

#include <QString>
#include <QList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QPointer>

class PdfPageContinuousIntervalSpecificator : public PdfPageRangeSpecificator
{
public:
    PdfPageContinuousIntervalSpecificator(QString documentPath, int firstPage, int lastPage, PdfUtil* doc);
    PdfPageContinuousIntervalSpecificator(QString documentPath, QString descriptor, PdfUtil* doc);
    ~PdfPageContinuousIntervalSpecificator() override;

    QList<int> getAllPages() const override;
    QString getDisplayText() const override;

    QString getDocumentPath() const override;
    PdfUtil* getDocument() const override;

private:
    int lowerPage, higherPage;
    QRegularExpression regxp = QRegularExpression("^(\\d+)\\-(\\d+)$");

    QString docPath;
    QPointer<PdfUtil> doc;
};

#endif // PDFPAGECONTINUOUSINTERVALSPECIFICATOR_H
