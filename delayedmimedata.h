#ifndef DELAYEDMIMEDATA_H
#define DELAYEDMIMEDATA_H

#include <QMimeData>
#include <QStringBuilder>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include "pdfpagerangespecificator.h"
#include "pdfnewdocument.h"

class DelayedMimeData : public QMimeData
{
    Q_OBJECT
public:
    DelayedMimeData(bool createTempFile = false, QString fileName = "", PdfPageRangeSpecificator* pageRange = nullptr);

private:
    QString tempFile;
    PdfPageRangeSpecificator* pdfRange;

    const QString externlFormat = QLatin1String("text/uri-list");
    bool createdTempFile = false;
    // QMimeData interface
public:
    QStringList formats() const override;

protected:
    QVariant retrieveData(const QString &mimetype, QVariant::Type preferredType) const override;
};

#endif // DELAYEDMIMEDATA_H
