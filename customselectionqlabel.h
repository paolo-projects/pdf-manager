#ifndef CUSTOMSELECTIONQLABEL_H
#define CUSTOMSELECTIONQLABEL_H

#include <QLabel>
#include <overlay.h>

class CustomSelectionQLabel : public QLabel
{
    Q_OBJECT
public:
    CustomSelectionQLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    CustomSelectionQLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
private:
    QPointer<Overlay> m_overlay;

    // QObject interface
public:
    bool event(QEvent *event) override;

    void showSelectionOverlay(bool selectionOverlay);
};

#endif // CUSTOMSELECTIONQLABEL_H
