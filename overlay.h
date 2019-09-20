#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QEvent>
#include <QPainter>
#include <QPointer>

class Overlay : public QWidget {
public:
    explicit Overlay(QWidget *parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter(this).fillRect(rect(), {0x42, 0x86, 0xf4, 128});
    }
};

class OverlayFactoryFilter : public QObject {
    QPointer<Overlay> m_overlay;
public:
    explicit OverlayFactoryFilter(QObject *parent = nullptr) : QObject(parent) {}
protected:
    bool eventFilter(QObject *obj, QEvent *ev) override {
        if (!obj->isWidgetType()) return false;
        auto w = static_cast<QWidget*>(obj);
        if (ev->type() == QEvent::MouseButtonPress) {

        }
        else if (ev->type() == QEvent::Resize) {
            if (m_overlay && m_overlay->parentWidget() == w)
                m_overlay->resize(w->size());
        }
        return false;
    }
};

#endif // OVERLAY_H
