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
            QPixmap dragPixmap = QPixmap::fromImage(*parWidget->getCurrentDoc()->GetPdfRenderedPageThumb(pageNum)->getImage());
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
