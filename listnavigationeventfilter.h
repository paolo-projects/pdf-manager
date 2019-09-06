#ifndef LISTNAVIGATIONEVENTFILTER_H
#define LISTNAVIGATIONEVENTFILTER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>

class ListNavigationEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit ListNavigationEventFilter(QObject *parent = nullptr);

signals:
    void navigationArrowPressed(int);
public slots:

protected:
    bool eventFilter(QObject *obj, QEvent* event);
};

#endif // LISTNAVIGATIONEVENTFILTER_H
