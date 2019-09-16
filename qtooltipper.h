#ifndef QTOOLTIPPER_H
#define QTOOLTIPPER_H

#include <QObject>
#include <QAbstractItemView>
#include <QHelpEvent>
#include <QToolTip>

class QToolTipper : public QObject
{
 Q_OBJECT
public:
 explicit QToolTipper(QObject* parent = nullptr)
 : QObject(parent)
 {
 }

protected:
 bool eventFilter(QObject* obj, QEvent* event) override
 {
     if (event->type() == QEvent::ToolTip)
     {
         QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj->parent());
         if (!view)
         {
            return false;
         }

         QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
         QPoint pos = helpEvent->pos();
         QModelIndex index = view->indexAt(pos);
         if (!index.isValid())
            return false;

         QString itemText = view->model()->data(index).toString();
         QString itemTooltip = view->model()->data(index, Qt::ToolTipRole).toString();

         QFontMetrics fm(view->font());
         int itemTextWidth = fm.width(itemText);
         QRect rect = view->visualRect(index);
         int rectWidth = rect.width();

         if ((itemTextWidth > rectWidth) && !itemTooltip.isEmpty())
         {
            QToolTip::showText(helpEvent->globalPos(), itemTooltip, view, rect);
         }
         else
         {
            QToolTip::hideText();
         }

         return true;
     }
     return false;
    }
};

#endif // QTOOLTIPPER_H
