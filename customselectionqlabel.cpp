#include "customselectionqlabel.h"

CustomSelectionQLabel::CustomSelectionQLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{

}

CustomSelectionQLabel::CustomSelectionQLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text, parent, f)
{

}

void CustomSelectionQLabel::showSelectionOverlay(bool selectionOverlay)
{
    if(selectionOverlay)
    {
        if (!m_overlay) m_overlay = new Overlay;
        m_overlay->setParent(this);
        m_overlay->resize(size());
        m_overlay->show();
    } else {
        if(m_overlay && m_overlay->isVisible())
            m_overlay->hide();
    }
}

bool CustomSelectionQLabel::event(QEvent *event)
{
    if (event->type() == QEvent::Resize) {
                if (m_overlay && m_overlay->parentWidget() == this)
                    m_overlay->resize(size());
    }
    return QLabel::event(event);
}
