#ifndef HINTWIDGET_H
#define HINTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QFont>
#include <QPointer>
#include <QEvent>

class HintWidget : public QWidget {
public:
    explicit HintWidget(QString hintText = "", QWidget *parent = nullptr) : QWidget(parent), hintText(hintText) {
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
    void setHintVisible(bool visible) { hintVisible = visible; }
    const QFont& font() const { return thisFont; }
    void setFont(QFont newFont) { thisFont = newFont; }
    const QColor& color() const { return textColor; }
    void setColor(QColor color) { textColor = color; }
    QString hint() const { return hintText; }
    void setHint(QString hint) { hintText = hint; }
protected:
    void paintEvent(QPaintEvent *) override {
        if(hintVisible)
        {
            QPainter pnt(this);
            pnt.setFont(thisFont);
            pnt.setPen(textColor);
            pnt.drawText(rect().marginsRemoved(QMargins(16, 16, 16, 16)), Qt::AlignCenter | Qt::TextWordWrap, hintText);
        }
    }
private:
    QString hintText;
    bool hintVisible = true;
    QFont thisFont = QFont();
    QColor textColor = QColor::fromRgb(0x33, 0x33, 0x33);
};

class HintWidgetFactoryFilter : public QObject {
    QPointer<HintWidget> m_HintWidget;
public:
    explicit HintWidgetFactoryFilter(HintWidget* hintWidget, QObject *parent = nullptr) : QObject(parent), m_HintWidget(hintWidget) {}
protected:
    bool eventFilter(QObject *obj, QEvent *ev) override {
        if (!obj->isWidgetType()) return false;
        auto w = static_cast<QWidget*>(obj);if (ev->type() == QEvent::Resize) {
            if (m_HintWidget && m_HintWidget->parentWidget() == w)
                m_HintWidget->resize(w->size());
        }
        return false;
    }
};

#endif // HINTWIDGET_H
