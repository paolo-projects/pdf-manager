#ifndef CUSTOMDRAGLISTVIEW_H
#define CUSTOMDRAGLISTVIEW_H

#include <QListView>
#include <QDrag>

#include "pdfpixmappainter.h"
#include "pdfpagerangespecificator.h"

class CustomDragListView : public QListView
{
    Q_OBJECT
public:
    CustomDragListView(QWidget* parent = nullptr);

    void setCurrentlyLoadedDocument(PdfUtil* document);
protected:
    void startDrag(Qt::DropActions supportedActions) override;

private:
    PdfUtil* currentlyLoadedDocument = nullptr;
};

#endif // CUSTOMDRAGLISTVIEW_H
