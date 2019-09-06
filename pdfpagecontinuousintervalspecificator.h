#ifndef PDFPAGECONTINUOUSINTERVALSPECIFICATOR_H
#define PDFPAGECONTINUOUSINTERVALSPECIFICATOR_H

#include "pdfpagerangespecificator.h"

#include <QString>
#include <QList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class PdfPageContinuousIntervalSpecificator : public PdfPageRangeSpecificator
{
public:
    PdfPageContinuousIntervalSpecificator(int firstPage, int lastPage);
    PdfPageContinuousIntervalSpecificator(QString descriptor);
    ~PdfPageContinuousIntervalSpecificator() override;

    QList<int> getAllPages() const override;
    QString getDisplayText() const override;

private:
    int lowerPage, higherPage;
    QRegularExpression regxp = QRegularExpression("^(\\d+)\\-(\\d+)$");
};

#endif // PDFPAGECONTINUOUSINTERVALSPECIFICATOR_H
