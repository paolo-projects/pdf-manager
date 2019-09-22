#ifndef PAGEGRIDWIDGET_H
#define PAGEGRIDWIDGET_H

#include <cmath>

#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QGraphicsView>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QAbstractSlider>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QItemSelectionModel>
#include <QRubberBand>
#include <QScrollBar>

#include "pdfutil.h"
#include "pdfrenderedpage.h"
#include "progsettings.h"
#include "pdfsinglepagespecificator.h"
#include "drageventfilter.h"
#include "delayedaction.h"
#include "customselectionqlabel.h"

class PageGridWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PageGridWidget(QWidget *parent = nullptr);
    ~PageGridWidget();

    void displayDocPages(PdfUtil* doc);

    void removeAllWidgets();

    int indexOf(QLabel* lbl);
    PdfUtil* getCurrentDoc();
    void navigateToPage(int pageNum);
    QSet<int> getSelectedPages() const;
    void clearSelectedPages();
signals:

public slots:
    void onScrollbarChange(int value);
    void onTimerEnd(int);
private:
    QScrollArea* scrollArea;
    QWidget* scrollAreaWidget;
    QGridLayout* gridLayout;
    QList<CustomSelectionQLabel*> displayedPictures;
    QVBoxLayout* mainLayout;

    int itemWidth, itemHeight;

    int columnsPerRow = 3;

    QList<QPixmap> loadedPixmaps;
    PdfUtil* currDoc;
    QVector<bool> renderedPages;

    bool selectable = true;
    QSet<int> selectedPages;
    QRubberBand currentRubberBand;
    QPoint mouseSelectionOrigin;

    // Methods

    QPair<int, int> getPreferredDisplayRange();
    QPair<int, int> getDisplayRange();

    QPair<int,int> lastDisplayRange;
    bool compareDisplayRanges(QPair<int,int>& displayRange, QPair<int,int>& prefDisplayRange);

    void repaintThumbnails();

    void paintText(QPixmap& pixmap, QString text);

    bool isSelectable() const;
    void setIsSelectable(bool isSelectable);
    void decorate(CustomSelectionQLabel* widget, bool decorate);

protected:
    void resizeEvent(QResizeEvent *event) override;
    DragEventFilter* dragEventFilter;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // PAGEGRIDWIDGET_H
