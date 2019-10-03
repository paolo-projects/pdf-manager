#ifndef DELAYEDACTION_H
#define DELAYEDACTION_H

#include <QTimer>
#include <QObject>
#include <QVariant>

/**
 * @brief The DelayedAction class is used to schedule a delayed action.
 * It's a kamikaze class, so it will auto-delete after emitting the delayEnd signal.
 */
class DelayedAction : public QObject
{
    Q_OBJECT
public:
    DelayedAction(int data) : data(data)
    {
        timer.reset(new QTimer());
        timer->setSingleShot(true);
        connect(timer.data(), SIGNAL(timeout()), this, SLOT(timerEnd()));
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
        delete this;
    }
private:
    QScopedPointer<QTimer> timer;
    int data;
};

#endif // DELAYEDACTION_H
