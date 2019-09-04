#ifndef PDFEXCEPTION_H
#define PDFEXCEPTION_H

#include <QException>

class PdfException : public QException
{
public:
    PdfException(QString const& message) :
        message(message)
    {}

    virtual ~PdfException()
    {

    }

    void raise() const { throw *this; }
    PdfException *clone() const { return new PdfException(*this); }

    QString getMessage() const
    {
        return message;
    }
private:
    QString message;
};

#endif // PDFEXCEPTION_H
