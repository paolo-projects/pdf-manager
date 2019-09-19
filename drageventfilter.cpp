#include "drageventfilter.h"
#include "pagegridwidget.h"

DragEventFilter::DragEventFilter(PageGridWidget *parentWidget)
    : parWidget(parentWidget)
{

}



bool DragEventFilter::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QLabel* lbl = static_cast<QLabel*>(object);

            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;

            QByteArray pointersData;
            QDataStream pointersStream(&pointersData, QIODevice::WriteOnly);

            pointersStream << -1;

            int pageNum = parWidget->indexOf(lbl);
            pageNum = (pageNum >= 0) ? pageNum : 0;

            PdfSinglePageSpecificator* rangedata = new PdfSinglePageSpecificator(parWidget->getCurrentDoc()->GetDocPath(), pageNum, parWidget->getCurrentDoc());

            intptr_t p = reinterpret_cast<intptr_t>(rangedata);
            pointersStream << p;

            mimeData->setData(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE, pointersData);

            drag->setMimeData(mimeData);
            QPixmap dragPixmap(*lbl->pixmap());
            drag->setPixmap(dragPixmap);

            drag->exec();
            return true;
        }
    }

    return false;
}
