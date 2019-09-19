#include "pdfutil.h"

PdfUtil::PdfUtil(QString path)
    :
      docPath(path)
{
    pdf_ctx = fz_new_context(nullptr, nullptr, FZ_STORE_UNLIMITED);
    if(!pdf_ctx)
        throw PdfException("Failed to create context.");

    fz_try(pdf_ctx)
            fz_register_document_handlers(pdf_ctx);
    fz_catch(pdf_ctx)
    {
        const char* error = fz_caught_message(pdf_ctx);
        fz_drop_context(pdf_ctx);
        pdf_ctx = nullptr;
        throw PdfException("Can't register document handlers. "+QString(error));
    }

    fz_try(pdf_ctx)
            pdf_doc = fz_open_document(pdf_ctx, path.toLocal8Bit().data());
    fz_catch(pdf_ctx)
    {
        const char* error = fz_caught_message(pdf_ctx);
        fz_drop_context(pdf_ctx);
        pdf_ctx = nullptr;
        throw PdfException("Can't open document. "+QString(error));
    }

    fz_try(pdf_ctx)
            pageCount = fz_count_pages(pdf_ctx, pdf_doc);
    fz_catch(pdf_ctx)
    {
        const char* error = fz_caught_message(pdf_ctx);
        fz_drop_document(pdf_ctx, pdf_doc);
        pdf_doc = nullptr;
        fz_drop_context(pdf_ctx);
        pdf_ctx = nullptr;
        throw PdfException("Can't count pages in document. "+QString(error));
    }

    if(pageCount <= 0)
    {
        fz_drop_document(pdf_ctx, pdf_doc);
        pdf_doc = nullptr;
        fz_drop_context(pdf_ctx);
        pdf_ctx = nullptr;
        throw PdfException("Empty document.");
    }

    pdf_matrix = fz_scale(zoom / 100.0f, zoom / 100.0f);
    pdf_matrix = fz_pre_rotate(pdf_matrix, rotation);

    pdf_matrix_thumb = fz_scale(30/100.0f, 30/100.0f);
    pdf_matrix_thumb = fz_pre_rotate(pdf_matrix_thumb, rotation);

    docName = QFileInfo(docPath).fileName();
}

PdfUtil::~PdfUtil()
{
    /*if(pdfDoc != nullptr)
    {
        delete pdfDoc;
    }*/

    if(pdf_doc != nullptr)
        fz_drop_document(pdf_ctx, pdf_doc);

    if(pdf_ctx != nullptr)
        fz_drop_context(pdf_ctx);
}

int PdfUtil::GetPageCount() const
{
    return pageCount;
}

PdfRenderedPage* PdfUtil::GetPdfRenderedPage(int pageNum)
{
    if(pageNum < 0 || pageNum >= pageCount)
        throw PdfException("Page requested out of bounds.");

    fz_try(pdf_ctx)
            pdf_pixmap = fz_new_pixmap_from_page_number(pdf_ctx, pdf_doc, pageNum, pdf_matrix, fz_device_rgb(pdf_ctx), 0);
    fz_catch(pdf_ctx)
    {
        const char* error = fz_caught_message(pdf_ctx);
        fz_drop_document(pdf_ctx, pdf_doc);
        pdf_doc = nullptr;
        fz_drop_context(pdf_ctx);
        pdf_ctx = nullptr;
        throw PdfException("Can't render page from document. "+QString(error));
    }

    /*QVector<unsigned char> returnData;

    for (int y = 0; y < pdf_pixmap->h; ++y)
    {
        unsigned char *p = &pdf_pixmap->samples[y * pdf_pixmap->stride];
        for (int x = 0; x < pdf_pixmap->w; ++x)
        {
            returnData.append(p[0]);
            returnData.append(p[1]);
            returnData.append(p[2]);
            returnData.append(0);
            //printf("%3d %3d %3d", p[0], p[1], p[2]);
            p += pdf_pixmap->n;
        }
        printf("\n");
    }*/

    PdfRenderedPage* pdfPage = new PdfRenderedPage(pdf_pixmap->samples, pdf_pixmap->h * pdf_pixmap->stride, pdf_pixmap->stride, pdf_pixmap->w, pdf_pixmap->h);

    fz_drop_pixmap(pdf_ctx, pdf_pixmap);

    return pdfPage;
}

PdfRenderedPage* PdfUtil::GetPdfRenderedPageThumb(int pageNum)
{
    if(pageNum < 0 || pageNum >= pageCount)
        throw PdfException("Page requested out of bounds.");

    fz_try(pdf_ctx)
            pdf_pixmap = fz_new_pixmap_from_page_number(pdf_ctx, pdf_doc, pageNum, pdf_matrix_thumb, fz_device_rgb(pdf_ctx), 0);
    fz_catch(pdf_ctx)
    {
        const char* error = fz_caught_message(pdf_ctx);
        fz_drop_document(pdf_ctx, pdf_doc);
        pdf_doc = nullptr;
        fz_drop_context(pdf_ctx);
        pdf_ctx = nullptr;
        throw PdfException("Can't render page from document. "+QString(error));
    }

    /*QVector<unsigned char> returnData;

    for (int y = 0; y < pdf_pixmap->h; ++y)
    {
        unsigned char *p = &pdf_pixmap->samples[y * pdf_pixmap->stride];
        for (int x = 0; x < pdf_pixmap->w; ++x)
        {
            returnData.append(p[0]);
            returnData.append(p[1]);
            returnData.append(p[2]);
            returnData.append(0);
            //printf("%3d %3d %3d", p[0], p[1], p[2]);
            p += pdf_pixmap->n;
        }
        printf("\n");
    }*/

    PdfRenderedPage* pdfPage = new PdfRenderedPage(pdf_pixmap->samples, pdf_pixmap->h * pdf_pixmap->stride, pdf_pixmap->stride, pdf_pixmap->w, pdf_pixmap->h);

    fz_drop_pixmap(pdf_ctx, pdf_pixmap);

    return pdfPage;
}

QString PdfUtil::GetDocName() const
{
    return docName;
}

QString PdfUtil::GetDocPath() const
{
    return docPath;
}
