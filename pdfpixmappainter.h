#ifndef PDFPIXMAPPAINTER_H
#define PDFPIXMAPPAINTER_H

#include <QPixmap>
#include <QPainter>

#include "pdfutil.h"
#include "pdfrenderedpage.h"
#include "pdfpagerangespecificator.h"

class PDFPixmapPainter
{
public:
    static QPixmap getSingleDocumentPixmap(PdfUtil* pdfUtil, int pageNum);
    static QPixmap getPageRangePixmap(PdfUtil* pdfUtil, int firstPage, int lastPage);

private:
    static constexpr double pageAlpha = 0.7;
    static constexpr int docWidth = 210, docHeight = 297;
    static constexpr int pageOffsetInMultiplePages = docWidth / 40;
    static const QColor borderPenColorTransparent, borderPenColorSolid;

    static void paintRect(QPoint topLeft, QPoint bottomRight, QPainter& painter, QImage* image = nullptr);
};

#endif // PDFPIXMAPPAINTER_H
