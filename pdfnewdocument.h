#ifndef PDFNEWDOCUMENT_H
#define PDFNEWDOCUMENT_H

#include <QString>

#include "mupdf/pdf.h"
#include "mupdf/fitz.h"

#include "pdfexception.h"
#include "pdfutil.h"

class PdfUtil::PdfNewDocument
    {
    public:
        PdfNewDocument(PdfUtil* parent, QString fileName);
        ~PdfNewDocument();
        void addPageFromParent(int pageNum);
        void Save();
    private:
        QString fileName;
        PdfUtil* pdfUtil;
        fz_context* ctx;
        pdf_document* doc_des = nullptr;
        pdf_document* doc_src = nullptr;
        pdf_write_options opts = pdf_default_write_options;
        pdf_graft_map* graft_map = nullptr;
        QString flags = "";

        bool localContext = false;
        int addedPages = 0;
};
#endif // PDFNEWDOCUMENT_H
