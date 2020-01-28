/***************************************************************************
 *                                                                         *
 *   File created: 28/01/2020                                                  *
 *   Copyright (C) 2020 by Paolo Infante                                 *
 *                                                                         *
 *   https://github.com/paolo-projects                                     *
 *                                                                         *
 ***************************************************************************/
#include "pdfimagepagespecificator.h"

PdfImagePageSpecificator::PdfImagePageSpecificator(QString imagePath)
    :   imgPath(imagePath)
{
}

PdfImagePageSpecificator::~PdfImagePageSpecificator()
{

}

QList<int> PdfImagePageSpecificator::getAllPages() const
{
    return QList<int>{0};
}

QString PdfImagePageSpecificator::getDisplayText() const
{
    return QFileInfo(imgPath).filePath();
}

QString PdfImagePageSpecificator::getDocumentPath() const
{
    return imgPath;
}

PdfUtil *PdfImagePageSpecificator::getDocument() const
{
    return nullptr;
}

bool PdfImagePageSpecificator::isImage() const
{
    return true;
}
