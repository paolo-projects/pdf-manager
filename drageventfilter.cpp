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

            QByteArray pointersData;
            QDataStream pointersStream(&pointersData, QIODevice::WriteOnly);

            pointersStream << -1;

            PdfPageRangeSpecificator* rangedata;

            QPixmap dragPixmap;

            auto selection = parWidget->getSelectedPages();
            if(selection.length()>0)
            {
                std::sort(selection.begin(), selection.end(), std::less<int>());
                rangedata = new PdfPageContinuousIntervalSpecificator(parWidget->getCurrentDoc()->GetDocPath(), selection.first(), selection.last(), parWidget->getCurrentDoc());
                dragPixmap = PDFPixmapPainter::getPageRangePixmap(parWidget->getCurrentDoc(), selection.first(), selection.last());
            } else {
                int pageNum = parWidget->indexOf(lbl);
                pageNum = (pageNum >= 0) ? pageNum : 0;
                rangedata = new PdfSinglePageSpecificator(parWidget->getCurrentDoc()->GetDocPath(), pageNum, parWidget->getCurrentDoc());
                dragPixmap = QPixmap::fromImage(*parWidget->getCurrentDoc()->GetPdfRenderedPageThumb(pageNum)->getImage());
            }

            // This subclass allows dropping the pages directly to OS filesystem, allowing a quick creation of pdf with small subsets of pages
            // However, this has been tested with Window, but not with other OS like Linux or Mac

            DelayedMimeData *mimeData = new DelayedMimeData(true, "extract-" % parWidget->getCurrentDoc()->GetDocName(), rangedata);

            intptr_t p = reinterpret_cast<intptr_t>(rangedata);
            pointersStream << p;

            mimeData->setData(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE, pointersData);

            drag->setMimeData(mimeData);

            QPainter painter(&dragPixmap);

            QPoint topLeft(0,0);
            QPoint bottomRight(dragPixmap.size().width(), dragPixmap.size().height());
            painter.setPen(Qt::black);

            painter.drawLine(topLeft.x(), topLeft.y(), bottomRight.x() - 1, topLeft.y()); // Top border
            painter.drawLine(bottomRight.x() - 1, topLeft.y(), bottomRight.x() - 1, bottomRight.y() - 1); // Right border
            painter.drawLine(bottomRight.x() - 1, bottomRight.y() - 1, topLeft.x(), bottomRight.y() - 1); // Bottom border
            painter.drawLine(topLeft.x(), bottomRight.y() - 1, topLeft.x(), topLeft.y()); // Left border

            painter.end();

            drag->setPixmap(dragPixmap);

            drag->exec();
            return true;
        }
    }

    return false;
}
