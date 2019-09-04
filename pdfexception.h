#ifndef PDFEXCEPTION_H
#define PDFEXCEPTION_H

#include <QException>

class PdfException : public QException
{
public:
    PdfException(const char* message) : msg(message) {}
    const char* what() const override { return msg; }

private:
    const char* msg;
};

#endif // PDFEXCEPTION_H
