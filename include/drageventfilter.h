#ifndef DRAGEVENTFILTER_H
#define DRAGEVENTFILTER_H

#include <QPainter>
#include <QDataStream>
#include <QStandardPaths>
#include <QPointer>

#include "pdfmultiplepagesspecificator.h"
#include "pdfpixmappainter.h"
#include "rawpointer.h"
#include "pdfnewdocument.h"

class PageGridWidget;

// Event watcher
class DragEventFilter : public QObject
{
public:
    DragEventFilter(PageGridWidget* parentWidget);
    bool eventFilter(QObject* object, QEvent* event);

private:
    QPointer<PageGridWidget> parWidget;
};

#endif // DRAGEVENTFILTER_H
