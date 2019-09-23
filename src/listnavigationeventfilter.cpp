#include "listnavigationeventfilter.h"

ListNavigationEventFilter::ListNavigationEventFilter(QObject *parent) : QObject(parent)
{

}

bool ListNavigationEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        int key = keyEvent->key();
        if(key == Qt::Key_Down || key == Qt::Key_Up)
        {
            emit navigationArrowPressed(key);
        }
    }
    return false;
}
