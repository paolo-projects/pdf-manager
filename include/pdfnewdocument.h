#ifndef PDFNEWDOCUMENT_H
#define PDFNEWDOCUMENT_H

#include <QString>
#include <QImage>
#include <QDebug>
#include <QTemporaryFile>

#include "mupdf/pdf.h"
#include "mupdf/fitz.h"

#include "pdfexception.h"
#include "pdfutil.h"
#include "pdfpagerangespecificator.h"

class PdfUtil::PdfNewDocument
    {
    public:
        PdfNewDocument(QString fileName);
        ~PdfNewDocument();
        void addPagesFromRange(PdfPageRangeSpecificator* range);
        void addPageFromImage(QString image_path);
        void Save();
    private:
        QString fileName;
        fz_context* ctx;
        pdf_document* doc_des = nullptr;
        pdf_write_options opts = pdf_default_write_options;
        pdf_graft_map* graft_map = nullptr;
        QString flags = "";

        bool localContext = false;
        int addedPages = 0;
        int imgcounter = 0;

        void addPageFromParent(pdf_document* doc_src, int pageNum);
        static constexpr int A4_w = 595;
        static constexpr int A4_h = 842;
};
#endif // PDFNEWDOCUMENT_H

