#ifndef PDFNEWDOCUMENT_H
#define PDFNEWDOCUMENT_H

#include <QString>

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

        void addPageFromParent(pdf_document* doc_src, int pageNum);
};
#endif // PDFNEWDOCUMENT_H

