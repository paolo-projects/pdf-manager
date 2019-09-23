#ifndef DELAYEDACTION_H
#define DELAYEDACTION_H

#include <QTimer>
#include <QObject>
#include <QVariant>

class DelayedAction : public QObject
{
    Q_OBJECT
public:
    DelayedAction(int data) : data(data)
    {
        timer = new QTimer();
        timer->setSingleShot(true);
        connect(timer, SIGNAL(timeout()), this, SLOT(timerEnd()));
    }
    void start(int delayMs)
    {
        timer->start(delayMs);
    }
signals:
    void delayEnd(int data);

private slots:
    void timerEnd()
    {
        emit delayEnd(data);
        delete timer;
        delete this;
    }
private:
    QTimer* timer;
    int data;
};

#endif // DELAYEDACTION_H
