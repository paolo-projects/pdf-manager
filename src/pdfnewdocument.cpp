#include "pdfnewdocument.h"

PdfUtil::PdfNewDocument::PdfNewDocument(QString fileName)
    :
      fileName(fileName)
{
    QFile newFile(fileName);
    if(newFile.exists())
        newFile.remove();

    ctx = fz_new_context(nullptr, nullptr, FZ_STORE_UNLIMITED);
    if(!ctx)
        throw PdfException("Failed to create context.");

    pdf_parse_write_options(ctx, &opts, flags.toLocal8Bit());

    fz_try(ctx)
    {
        doc_des = pdf_create_document(ctx);

    }
    fz_catch(ctx)
    {
        fz_flush_warnings(ctx);
        fz_drop_context(ctx);
        throw PdfException("Cannot create PDF document.");
    }
}

PdfUtil::PdfNewDocument::~PdfNewDocument()
{
    pdf_drop_document(ctx, doc_des);
    fz_drop_context(ctx);
}

void PdfUtil::PdfNewDocument::addPagesFromRange(PdfPageRangeSpecificator* range)
{
    pdf_document* doc_src = pdf_open_document(ctx, range->getDocumentPath().toLocal8Bit());

    for(int page : range->getAllPages())
    {
        graft_map = pdf_new_graft_map(ctx, doc_des);
        addPageFromParent(doc_src, page);
        pdf_drop_graft_map(ctx, graft_map);
    }

    pdf_drop_document(ctx, doc_src);
}

void PdfUtil::PdfNewDocument::addPageFromParent(pdf_document* doc_src, int pageNum)
{
    pdf_obj *page_ref;
    pdf_obj *page_dict = NULL;
    pdf_obj *obj;
    pdf_obj *ref = NULL;
    int i;

    /* Copy as few key/value pairs as we can. Do not include items that reference other pages. */
    static pdf_obj * const copy_list[] = {
        PDF_NAME(Contents),
        PDF_NAME(Resources),
        PDF_NAME(MediaBox),
        PDF_NAME(CropBox),
        PDF_NAME(BleedBox),
        PDF_NAME(TrimBox),
        PDF_NAME(ArtBox),
        PDF_NAME(Rotate),
        PDF_NAME(UserUnit)
    };

    fz_var(ref);
    fz_var(page_dict);

    fz_try(ctx)
    {
        page_ref = pdf_lookup_page_obj(ctx, doc_src, pageNum);
        pdf_flatten_inheritable_page_items(ctx, page_ref);

        /* Make a new page object dictionary to hold the items we copy from the source page. */
        page_dict = pdf_new_dict(ctx, doc_des, 4);

        pdf_dict_put(ctx, page_dict, PDF_NAME(Type), PDF_NAME(Page));

        for (i = 0; i < nelem(copy_list); i++)
        {
            obj = pdf_dict_get(ctx, page_ref, copy_list[i]);
            if (obj != NULL)
                pdf_dict_put_drop(ctx, page_dict, copy_list[i], pdf_graft_mapped_object(ctx, graft_map, obj));
        }

        /* Add the page object to the destination document. */
        ref = pdf_add_object(ctx, doc_des, page_dict);

        /* Insert it into the page tree. */
        pdf_insert_page(ctx, doc_des, addedPages, ref);
        addedPages++;
    }
    fz_always(ctx)
    {
        pdf_drop_obj(ctx, page_dict);
        pdf_drop_obj(ctx, ref);
    }
    fz_catch(ctx)
    {
        throw PdfException("Can't add page "+QString::number(pageNum)+" to PDF file.");
    }
}

// TEMPORARY CLASS
#define mmin(x, y) ( (x) > (y) ? (y) : (x) )
class BufferedImageData : public QIODevice
{
public:
    BufferedImageData()
    {

    }
    ~BufferedImageData()
    {

    }
    bool atEnd() const override { return false; }
    qint64 bytesAvailable() const override { return bdata.size(); }
    bool isSequential() const override { return true; }
    bool putChar(char c) {}
    bool seek(qint64 pos) override { return false; }
    qint64 size() const override { return bdata.size(); }
    qint64 write(const char* data, qint64 maxSize)
    {
        auto initsize = bdata.size();
        bdata.resize(initsize + maxSize);
        memcpy(&bdata[initsize], data, maxSize);
        return maxSize;
    }
    qint64 write(const char* data)
    {
        auto maxsize = qstrlen(data);
        write(data, maxsize);
    }
    qint64 write(const QByteArray& byteArray)
    {
        auto maxsize = byteArray.size();
        write(byteArray.constData(), maxsize);
    }
    const unsigned char* constData() const
    {
        return bdata.constData();
    }
    unsigned char* data()
    {
        return bdata.data();
    }
    qint64 readData(char *data, qint64 maxlen) override
    {
        auto szlen = mmin(maxlen, bdata.size());
        memcpy(data, bdata.data(), szlen);
        return szlen;
    }
    qint64 writeData(const char *data, qint64 len) override
    {
        write(data, len);
        return len;
    }
private:
    QVector<unsigned char> bdata;
};
#undef mmin

void PdfUtil::PdfNewDocument::addPageFromImage(QString image_path)
{
    if(image_path.length() > 0)
    {
        fz_image* img = NULL;
        fz_buffer* nres = NULL;
        pdf_obj* img_obj = NULL;
        pdf_obj* page_resx = NULL;

        fz_try(ctx)
        {
            const char* pdfpageimg_templ = " q %g %g %g %g %g %g cm /%s Do Q ";
            std::string _imgname = "imgObj" + std::to_string(imgcounter);

            // Conversion to JPG needed to ensure the inserted image will be compressed
            // Otherwise huge PDF files would be created
            if(QFileInfo(image_path).suffix() != "jpg" && QFileInfo(image_path).suffix() != "jpeg")
            {
                qDebug() << "Original image size: " << QFileInfo(image_path).size()/1024 << " KB\n";
                QImage img_src(image_path);
                QTemporaryFile imgfile("pdfman-XXXXXX.jpg");
                if(imgfile.open())
                {
                    img_src.save(imgfile.fileName(), "JPG");

                    qDebug() << "JPG image size: " << imgfile.size()/1024 << " KB at " << imgfile.fileName() << "\n";

                    img = fz_new_image_from_file(ctx, imgfile.fileName().toLocal8Bit());
                }
                else
                {
                    img = fz_new_image_from_file(ctx, image_path.toLocal8Bit());
                }
            }
            else
            {
                img = fz_new_image_from_file(ctx, image_path.toLocal8Bit());
            }
            fz_pixmap* pixm = fz_get_pixmap_from_image(ctx, img, NULL, NULL, 0, 0);

            if(pixm->alpha == 1)
            {
                fz_color_params color_params = {0};
                fz_pixmap* pm = fz_convert_pixmap(ctx, pixm, NULL, NULL, NULL, color_params, 1);
                pm->alpha = 0;
                pm->colorspace = fz_keep_colorspace(ctx, fz_device_gray(ctx));
                fz_image* mask = fz_new_image_from_pixmap(ctx, pm, NULL);
                fz_image* zimg = fz_new_image_from_pixmap(ctx, pixm, mask);
                fz_drop_image(ctx, img);
                img = zimg;
                zimg = NULL;
            }

            img_obj = pdf_add_image(ctx, doc_des, img);

            page_resx = pdf_add_object_drop(ctx, doc_des, pdf_new_dict(ctx, doc_des, 1));
            nres = fz_new_buffer(ctx, 50);

            fz_rect mediabox = fz_make_rect(0, 0, img->w, img->h);

            auto new_page = pdf_add_page(ctx, doc_des, mediabox, 0, page_resx, nres);
                    //pdf_new_dict(ctx, doc_des, 4);

            //pdf_page_transform(ctx, new_page, mdbox, mtx);

            pdf_dict_put(ctx, new_page, PDF_NAME(Type), PDF_NAME(Page));

           //auto page_resx = pdf_dict_get_inheritable(ctx, new_page, PDF_NAME(Resources));
            auto page_xobjs = pdf_dict_get(ctx, page_resx, PDF_NAME(XObject));
            if(!page_xobjs)
            {
                page_xobjs = pdf_new_dict(ctx, doc_des, 10);
                pdf_dict_putl_drop(ctx, new_page, page_xobjs, PDF_NAME(Resources), PDF_NAME(XObject), NULL);
            }

            pdf_dict_puts(ctx, page_xobjs, _imgname.c_str(), img_obj);

            fz_matrix mat = fz_make_matrix(img->w, 0.0f, 0.0f, img->h/*434.33235*/, 0.0f, 0/*255.83383*/);

            fz_append_printf(ctx, nres, pdfpageimg_templ, mat.a, mat.b, mat.c, mat.d, mat.e, mat.f,
                             _imgname.c_str());

            pdf_obj* contents = pdf_dict_get(ctx, new_page, PDF_NAME(Contents));
            pdf_obj* newconts = pdf_add_stream(ctx, doc_des, nres, NULL, 0);

            int overlay = 1;

            if (pdf_is_array(ctx, contents))
            {
                if (overlay)               // append new object
                    pdf_array_push_drop(ctx, contents, newconts);
                else                       // prepend new object
                    pdf_array_insert_drop(ctx, contents, newconts, 0);
            }
            else                           // make new array
            {
                pdf_obj *carr = pdf_new_array(ctx, doc_des, 5);
                if (overlay)
                {
                    if (contents) pdf_array_push(ctx, carr, contents);
                    pdf_array_push_drop(ctx, carr, newconts);
                }
                else
                {
                    pdf_array_push_drop(ctx, carr, newconts);
                    if (contents) pdf_array_push(ctx, carr, contents);
                }
                pdf_dict_put_drop(ctx, new_page, PDF_NAME(Contents), carr);
            }

            doc_des->dirty = 1;

            pdf_insert_page(ctx, doc_des, addedPages, new_page);

            addedPages++;
            imgcounter++;
        }
        fz_always(ctx)
        {
            pdf_drop_obj(ctx, page_resx);
            pdf_drop_obj(ctx, img_obj);
            fz_drop_buffer(ctx, nres);
            fz_drop_image(ctx, img);
        }
        fz_catch(ctx)
        {
            throw PdfException("Can't add image "+image_path+" to PDF File.");
        }
    }
}

void PdfUtil::PdfNewDocument::Save()
{
    fz_try(ctx)
        pdf_save_document(ctx, doc_des, fileName.toLocal8Bit(), &opts);
    fz_catch(ctx)
        throw PdfException("Can't save to path: "+fileName);
}
