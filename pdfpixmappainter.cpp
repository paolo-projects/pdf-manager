#include "pdfpixmappainter.h"

const QColor PDFPixmapPainter::borderPenColorTransparent = QColor(0, 0, 0, 127);
const QColor PDFPixmapPainter::borderPenColorSolid = QColor(0, 0, 0, 255);

QPixmap PDFPixmapPainter::getSingleDocumentPixmap(PdfUtil *pdfUtil, int pageNum)
{
    PdfRenderedPage* pdfPage = pdfUtil->GetPdfRenderedPage(pageNum);
    QImage* image = pdfPage->getImage();
    //QPixmap docPixmap = QPixmap::fromImage(*image).scaled(docWidth, docHeight, Qt::KeepAspectRatio);

    QPixmap output(QSize(docWidth, docHeight));
    output.fill(Qt::white);
    QPainter painter(&output);
    painter.setOpacity(pageAlpha);
    //painter.drawPixmap(0, 0, docPixmap);
    painter.setPen(borderPenColorTransparent);
    int right = docWidth;
    int bottom = docHeight;
    paintRect(QPoint(0, 0), QPoint(right, bottom), painter, image);
    /*
    painter.drawLine(0, 0, right, 0); // Top border
    painter.drawLine(right, 0, right, bottom); // Right border
    painter.drawLine(right, bottom, 0, bottom); // Bottom border
    painter.drawLine(0, bottom, 0, 0); // Left border*/
    painter.end();

    delete image;
    delete pdfPage;

    return output;
}

QPixmap PDFPixmapPainter::getPageRangePixmap(PdfUtil *pdfUtil, int firstPage, int lastPage)
{
    int middlePage = firstPage;

    if(std::abs(lastPage - firstPage) > 1)
    {
        middlePage = std::abs((lastPage - firstPage)/2);
    }

    PdfRenderedPage* pdfLastPage = pdfUtil->GetPdfRenderedPage(lastPage);
    PdfRenderedPage* pdfMiddlePage = pdfUtil->GetPdfRenderedPage(middlePage);
    PdfRenderedPage* pdfFirstPage = pdfUtil->GetPdfRenderedPage(firstPage);

    QImage* lastPageImage = pdfLastPage->getImage();
    QImage* middlePageImage = pdfMiddlePage->getImage();
    QImage* firstPageImage = pdfFirstPage->getImage();

    //QPixmap docPixmap = QPixmap::fromImage(*lastPageImage).scaled(docWidth, docHeight, Qt::KeepAspectRatio);

    QPixmap output(docWidth + 2*pageOffsetInMultiplePages, docHeight + 2*pageOffsetInMultiplePages);
    output.fill(Qt::transparent);
    QPainter painter(&output);

    painter.setOpacity(1.0);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(borderPenColorSolid);
    int right = docWidth - 1;
    int bottom = docHeight - 1;

    painter.fillRect(QRect(QPoint(0, 0), QPoint(right , bottom)), QBrush(Qt::white));
    paintRect(QPoint(0, 0), QPoint(right, bottom), painter, firstPageImage);

    painter.fillRect(QRect(QPoint(pageOffsetInMultiplePages, pageOffsetInMultiplePages), QPoint(right + pageOffsetInMultiplePages, bottom + pageOffsetInMultiplePages)), QBrush(Qt::white));
    paintRect(QPoint(pageOffsetInMultiplePages, pageOffsetInMultiplePages), QPoint(right + pageOffsetInMultiplePages, bottom + pageOffsetInMultiplePages), painter, middlePageImage);

    painter.fillRect(QRect(QPoint(2 * pageOffsetInMultiplePages, 2 * pageOffsetInMultiplePages), QPoint(right + 2 * pageOffsetInMultiplePages, bottom + 2 * pageOffsetInMultiplePages)), QBrush(Qt::white));
    //painter.drawPixmap(2 * pageOffsetInMultiplePages, 2 * pageOffsetInMultiplePages, docPixmap);
    paintRect(QPoint(2 * pageOffsetInMultiplePages, 2 * pageOffsetInMultiplePages), QPoint(right + 2 * pageOffsetInMultiplePages, bottom + 2 * pageOffsetInMultiplePages), painter, lastPageImage);

    /*painter.drawLine(0, 0, right, 0); // Top border
    painter.drawLine(right, 0, right, bottom); // Right border
    painter.drawLine(right, bottom, 0, bottom); // Bottom border
    painter.drawLine(0, bottom, 0, 0); // Left border*/
    painter.end();

    delete firstPageImage;
    delete middlePageImage;
    delete lastPageImage;

    delete pdfFirstPage;
    delete pdfMiddlePage;
    delete pdfLastPage;

    return output;
}

void PDFPixmapPainter::paintRect(QPoint topLeft, QPoint bottomRight, QPainter &painter, QImage* image)
{
    if (image != nullptr)
    {
        //painter.fillRect(QRect(topLeft, bottomRight), QBrush(Qt::white));
        painter.drawImage(QRect(topLeft, bottomRight), *image, image->rect());
    }

    painter.drawLine(topLeft.x(), topLeft.y(), bottomRight.x() - 1, topLeft.y()); // Top border
    painter.drawLine(bottomRight.x() - 1, topLeft.y(), bottomRight.x() - 1, bottomRight.y() - 1); // Right border
    painter.drawLine(bottomRight.x() - 1, bottomRight.y() - 1, topLeft.x(), bottomRight.y() - 1); // Bottom border
    painter.drawLine(topLeft.x(), bottomRight.y() - 1, topLeft.x(), topLeft.y()); // Left border
}


