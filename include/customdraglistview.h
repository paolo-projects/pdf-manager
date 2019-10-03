#ifndef CUSTOMDRAGLISTVIEW_H
#define CUSTOMDRAGLISTVIEW_H

#include <QListView>
#include <QDrag>
#include <QPointer>

#include "pdfpixmappainter.h"
#include "pdfpagerangespecificator.h"

class CustomDragListView : public QListView
{
    Q_OBJECT
public:
    CustomDragListView(QWidget* parent = nullptr);

    void setCurrentlyLoadedDocuments(QList<PdfUtil*>& document);
protected:
    void startDrag(Qt::DropActions supportedActions) override;

private:
    QList<PdfUtil*>* currentlyLoadedDocuments;
};

#endif // CUSTOMDRAGLISTVIEW_H
