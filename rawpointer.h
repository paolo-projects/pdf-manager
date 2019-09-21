#ifndef RAWPOINTER_H
#define RAWPOINTER_H

#include <QDataStream>

#include "pdfpagerangespecificator.h"

class RawPointer
{
public:
    RawPointer() = default;
    RawPointer(PdfPageRangeSpecificator* pnt) { pointer = reinterpret_cast<intptr_t>(pnt); }
    intptr_t pointer;

    friend QDataStream& operator << (QDataStream& out, const RawPointer& p) {
        char data[sizeof(intptr_t)];
        memcpy(data, &p.pointer, sizeof(intptr_t));
        out.writeRawData(data, sizeof(intptr_t));
        return out;
    }

    friend QDataStream& operator >> (QDataStream& out, RawPointer& p) {
        char data[sizeof(intptr_t)];
        out.readRawData(data, sizeof(intptr_t));
        memcpy(&p.pointer, data, sizeof(intptr_t));
        return out;
    }
};

#endif // RAWPOINTER_H
