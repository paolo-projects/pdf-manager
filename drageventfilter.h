#ifndef DRAGEVENTFILTER_H
#define DRAGEVENTFILTER_H

#include <QObject>

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
