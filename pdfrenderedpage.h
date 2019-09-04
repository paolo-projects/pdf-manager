#ifndef PDFRENDEREDPAGE_H
#define PDFRENDEREDPAGE_H

#include <QVector>
#include <QImage>

class PdfRenderedPage
{
public:
    PdfRenderedPage(QVector<unsigned char> imageData, int width, int height);
    PdfRenderedPage(unsigned char* imageData, size_t size, int stride, int width, int height);
    ~PdfRenderedPage();
    QImage* getImage();
    int getWidth() const;
    int getHeight() const;
private:
    QVector<unsigned char> vecData;
    uchar* data = nullptr;
    int width, height;
    int stride = -1;
};

#endif // PDFRENDEREDPAGE_H
