#ifndef CUSTOMDRAGTREEVIEW_H
#define CUSTOMDRAGTREEVIEW_H

#include <QTreeView>
#include <QDrag>

#include "pdfpixmappainter.h"
#include "pdfpagerangespecificator.h"
#include "treeitem.h"

class CustomDragTreeView : public QTreeView
{
    Q_OBJECT
public:
    CustomDragTreeView(QWidget* parent = nullptr);
    void setCurrentlyLoadedDocuments(QList<PdfUtil*>& loadedDocuments);
protected:
    void startDrag(Qt::DropActions supportedActions) override;
private:
    QList<PdfUtil*>* currentlyLoadedDocuments;


    // QObject interface
public:
    bool event(QEvent *event) override;
signals:
    void navigationArrowPressed(int);
};

#endif // CUSTOMDRAGTREEVIEW_H
