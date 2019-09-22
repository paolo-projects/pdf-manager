#include "delayedmimedata.h"

DelayedMimeData::DelayedMimeData(bool createTempFile, QString fileName, PdfPageRangeSpecificator* pageRange)
    : QMimeData(), pdfRange(pageRange)
{
    if(createTempFile)
    {
        tempFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + fileName;
        QFile::remove(tempFile);

        PdfUtil::PdfNewDocument newDoc(tempFile);
        newDoc.addPagesFromRange(pdfRange);
        newDoc.Save();

        createdTempFile = true;
    }
}

QStringList DelayedMimeData::formats() const
{
    return QMimeData::formats() << externlFormat;
}

QVariant DelayedMimeData::retrieveData(const QString &mimetype, QVariant::Type preferredType) const
{
    if( mimetype == externlFormat && createdTempFile)
    {
        qDebug() << "retrieveData requested";
        return QVariant::fromValue<QUrl>( QUrl::fromLocalFile(tempFile) );
    }
    else
        return QMimeData::retrieveData(mimetype, preferredType);
}
