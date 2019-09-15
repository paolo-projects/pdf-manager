#ifndef PDFUTIL_H
#define PDFUTIL_H

#include <QVector>
#include <QImage>
#include <QString>
#include <QException>
#include <QFileInfo>
#include <mupdf/fitz.h>

#include "pdfrenderedpage.h"
#include "pdfexception.h"

class PdfUtil
{
public:
    PdfUtil(QString path);
    ~PdfUtil();
    int GetPageCount() const;
    PdfRenderedPage*  GetPdfRenderedPage(int pageNum);
    QString GetDocName() const;
    QString GetDocPath() const;
    class PdfNewDocument;
private:
    QString docPath, docName;
    fz_context* pdf_ctx = nullptr;
    int pageCount = 0;
    fz_document* pdf_doc = nullptr;
    fz_pixmap* pdf_pixmap = nullptr;
    fz_matrix pdf_matrix;
    int zoom = 100;
    int rotation = 0;
};

#endif // PDFUTIL_H
