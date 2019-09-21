#ifndef DRAGEVENTFILTER_H
#define DRAGEVENTFILTER_H

#include <QObject>
#include <QPainter>

#include "pdfpagecontinuousintervalspecificator.h"
#include "pdfpixmappainter.h"
#include "delayedmimedata.h"

class PageGridWidget;

// Event watcher
class DragEventFilter : public QObject
{
public:
    DragEventFilter(PageGridWidget* parentWidget);
    bool eventFilter(QObject* object, QEvent* event);

private:
    PageGridWidget* parWidget;
};

#endif // DRAGEVENTFILTER_H
