#include "customdragtreeview.h"

CustomDragTreeView::CustomDragTreeView(QWidget* parent)
    :
      QTreeView(parent)
{

}

void CustomDragTreeView::startDrag(Qt::DropActions supportedActions)
{
    QModelIndexList indexes = selectedIndexes();
    if (indexes.count() > 0) {
        QMimeData *data = model()->mimeData(indexes);
        if (!data)
            return;

        if(indexes.length()>1)
        {
            //QAbstractItemView::startDrag(supportedActions);
            QRect rect;
            int pageNumF = 0, pageNumL = 0;


            QVariant m_data = model()->data(indexes.last(), Qt::DisplayRole);
            pageNumL = qvariant_cast<QString>(m_data).toInt() - 1;

            m_data = model()->data(indexes.first(), Qt::DisplayRole);
            pageNumF = qvariant_cast<QString>(m_data).toInt() - 1;

            auto parent = indexes.at(0).parent();
            PdfUtil* doc = currentlyLoadedDocuments->at(parent.row());
            if(doc!=nullptr)
            {
                QPixmap page = PDFPixmapPainter::getPageRangePixmap(doc, pageNumF, pageNumL);

                rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
                QDrag *drag = new QDrag(this);
                drag->setPixmap(page);
                drag->setMimeData(data);
                //drag->setHotSpot(QPoint(-48,-48)); //d->pressedPosition - rect.topLeft());
                Qt::DropAction defAction = Qt::IgnoreAction;
                if (defaultDropAction() != Qt::IgnoreAction && (supportedActions & defaultDropAction()))
                    defAction = defaultDropAction();
                else if (supportedActions & Qt::CopyAction && dragDropMode() != QAbstractItemView::InternalMove)
                    defAction = Qt::CopyAction;
                drag->exec(supportedActions, defAction);
            } else QTreeView::startDrag(supportedActions);
        } else {
            auto parent = indexes.at(0).parent();
            PdfUtil* doc = currentlyLoadedDocuments->at(parent.row());
            if(doc != nullptr)
            {
                QRect rect;
                QVariant m_data = model()->data(indexes.first(), Qt::EditRole);
                int pageNum = qvariant_cast<QString>(m_data).toInt() - 1;

                //PdfRenderedPage* pdfPage = currentlyLoadedDocument->GetPdfRenderedPage(pageNum);

                /*
                QImage* image = pdfPage->getImage();
                QPixmap pixmap = QPixmap::fromImage(*image).scaled(210, 297, Qt::KeepAspectRatio);

                QPixmap output(pixmap.size());
                output.fill(Qt::transparent);
                QPainter painter(&output);
                painter.setOpacity(0.7);
                painter.drawPixmap(0, 0, pixmap);
                painter.setPen(QColor(0, 0, 0, 127));
                int right = pixmap.width() - 1;
                int bottom = pixmap.height() - 1;
                painter.drawLine(0, 0, right, 0); // Top border
                painter.drawLine(right, 0, right, bottom); // Right border
                painter.drawLine(right, bottom, 0, bottom); // Bottom border
                painter.drawLine(0, bottom, 0, 0); // Left border
                painter.end();*/

                QPixmap page = PDFPixmapPainter::getSingleDocumentPixmap(doc, pageNum);

                rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
                QDrag *drag = new QDrag(this);
                drag->setPixmap(page);
                drag->setMimeData(data);
                //drag->setHotSpot(QPoint(-48,-48)); //d->pressedPosition - rect.topLeft());
                Qt::DropAction defAction = Qt::IgnoreAction;
                if (defaultDropAction() != Qt::IgnoreAction && (supportedActions & defaultDropAction()))
                    defAction = defaultDropAction();
                else if (supportedActions & Qt::CopyAction && dragDropMode() != QAbstractItemView::InternalMove)
                    defAction = Qt::CopyAction;
                drag->exec(supportedActions, defAction);

                /*delete image;
                delete pdfPage;*/
            } else QTreeView::startDrag(supportedActions);
        }
    }
}

bool CustomDragTreeView::event(QEvent *event)
{
    bool ret = QTreeView::event(event);

    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = reinterpret_cast<QKeyEvent*>(event);
        int key = keyEvent->key();
        if(key == Qt::Key_Down || key == Qt::Key_Up)
        {
            emit navigationArrowPressed(key);
        }
    }
    return ret;
}

void CustomDragTreeView::setCurrentlyLoadedDocuments(QList<PdfUtil *> &loadedDocuments)
{
    currentlyLoadedDocuments = &loadedDocuments;
}
