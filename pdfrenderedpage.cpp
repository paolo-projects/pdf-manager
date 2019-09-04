#include "pdfrenderedpage.h"

PdfRenderedPage::PdfRenderedPage(QVector<unsigned char> imageData, int width, int height)
    :
      vecData(imageData), data(imageData.data()), width(width), height(height)
{

}

PdfRenderedPage::PdfRenderedPage(unsigned char *imageData, size_t size, int stride, int width, int height)
    :
      width(width), height(height), stride(stride)
{
    data = new unsigned char[size];
    memcpy(data, imageData, size*sizeof(unsigned char));
}

PdfRenderedPage::~PdfRenderedPage()
{
    if (data!=nullptr)
        delete[] data;
}

QImage* PdfRenderedPage::getImage()
{
    if(vecData.empty())
        return new QImage(data, width, height, stride, QImage::Format_RGB888);
    else
        return new QImage(data, width, height, QImage::Format_RGB888);
}

int PdfRenderedPage::getWidth() const
{
    return width;
}

int PdfRenderedPage::getHeight() const
{
    return height;
}
