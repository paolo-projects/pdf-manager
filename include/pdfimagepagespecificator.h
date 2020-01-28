/***************************************************************************
 *                                                                         *
 *   File created: 28/01/2020                                                  *
 *   Copyright (C) 2020 by Paolo Infante                                 *
 *                                                                         *
 *   https://github.com/paolo-projects                                     *
 *                                                                         *
 ***************************************************************************/
#ifndef PDFIMAGEPAGESPECIFICATOR_H
#define PDFIMAGEPAGESPECIFICATOR_H

#include <QString>
#include <QList>
#include <QPointer>

#include "pdfpagerangespecificator.h"

class PdfImagePageSpecificator : public PdfPageRangeSpecificator
{
public:
    PdfImagePageSpecificator(QString imagePath);
    ~PdfImagePageSpecificator() override;

    QList<int> getAllPages() const override;
    QString getDisplayText() const override;

    QString getDocumentPath() const override;
    PdfUtil* getDocument() const override;
    bool isImage() const override;

private:
    QString imgPath;
};


#endif // PDFIMAGEPAGESPECIFICATOR_H
