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
            if(selection.size()>1)
            {
                auto selectionList = QList<int>::fromSet(selection);
                std::sort(selectionList.begin(), selectionList.end(), std::less<int>());

                rangedata = new PdfMultiplePagesSpecificator(parWidget->getCurrentDoc()->GetDocPath(), selectionList, parWidget->getCurrentDoc());
                dragPixmap = PDFPixmapPainter::getPageRangePixmap(parWidget->getCurrentDoc(), selectionList.first(), selectionList.last());
            } else {
                int pageNum = parWidget->indexOf(lbl);
                pageNum = (pageNum >= 0) ? pageNum : 0;
                rangedata = new PdfSinglePageSpecificator(parWidget->getCurrentDoc()->GetDocPath(), pageNum, parWidget->getCurrentDoc());
                dragPixmap = QPixmap::fromImage(*parWidget->getCurrentDoc()->GetPdfRenderedPageThumb(pageNum)->getImage());
            }

            // This code allows dropping the pages directly to OS filesystem, allowing a quick creation of pdf with small subsets of pages
            QMimeData* mimeData = new QMimeData();
            QString fName = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/extract-" + parWidget->getCurrentDoc()->GetDocName();

            {
                PdfUtil::PdfNewDocument newDoc(fName);
                newDoc.addPagesFromRange(rangedata);
                newDoc.Save();
            }

            pointersStream << RawPointer(rangedata);

            mimeData->setData(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE, pointersData);

            mimeData->setUrls(QList<QUrl>() << QUrl::fromLocalFile(fName));

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

            drag->exec(Qt::LinkAction | Qt::CopyAction);

            return true;
        }
    }

    return false;
}
