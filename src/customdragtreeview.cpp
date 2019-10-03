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
