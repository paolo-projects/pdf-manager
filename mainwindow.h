#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QStringListModel>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>
#include <QGLWidget>
#include <QStandardItemModel>
#include <QStandardItem>

#include "pdfutil.h"
#include "pdfpagerangespecificator.h"
#include "pdfsinglepagespecificator.h"
#include "pdfmultiplepagesspecificator.h"
#include "pdfpagecontinuousintervalspecificator.h"
#include "pdfexception.h"
#include "pdfnewdocument.h"
#include "progsettings.h"
#include "pdfrangesitemmodel.h"
#include "listnavigationeventfilter.h"
#include "openeddocumentstreemodel.h"
#include "htmldelegate.h"
#include "qtooltipper.h"

#include "aboutdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_action_Load_PDF_triggered();

    void on_pdfPagesList_doubleClicked(const QModelIndex &index);

    void on_pdfPagesList_activated(const QModelIndex &index);

    void on_pdfPagesList_clicked(const QModelIndex &index);

    void pdfPagesContextMenu(const QPoint& point);
    void pdfNewPagesContextMenu(const QPoint& point);

    void on_addSinglePageBtn_clicked();

    void on_addPageRangeBtn_clicked();

    void on_addMultiplePagesBtn_clicked();

    void on_action_Export_PDF_triggered();

    void hideProgressBar();

    void on_action_Add_all_triggered();

    void on_action_Delete_all_triggered();

    void on_action_Exit_triggered();

    void on_action_About_triggered();

    void pdfPagesArrowReceived(int key);
private:
    Ui::MainWindow *ui;
    PdfRenderedPage* displayedPage = nullptr;
    QImage* displayedImage = nullptr;
    QGraphicsScene* grScene = nullptr;
    QList<PdfUtil*> loadedDocuments;
    QGraphicsPixmapItem pixItem;
    QProgressBar* progBar;

    //QStringListModel* pdfPageListModel;
    OpenedDocumentsTreeModel* pdfPageListModel;

    QList<PdfPageRangeSpecificator*> pageRanges;
    PdfRangesItemModel* pdfPageRangesListModel;

    QTimer* timer = nullptr;

    QString defaultProgBarFormat;
    QString errorProgBarFormat =  "QProgressBar {"
                                  "background-color: #000000;"
                                  "}"
                                  ""
                                  "QProgressBar::chunk {"
                                  "background-color: #FF0000;"
                                  "}";

    void setCurrentlyDisplayedPage(int pageNum, int docIndex);
    void updatePdfPageList();
    void addPdfPageList(PdfUtil* doc);
    void resizeEvent(QResizeEvent* event) override;

    void startProgressBar(int maxValue, int initialValue = 0, bool showText = SETTINGS::DEFAULT_PROGBAR_DISPLAY_TEXT);
    void incrementProgressBar(int increment = 1);
    void completeProgressBar(bool error = false, int delay_ms = SETTINGS::DEFAULT_PROGBAR_DELAY_BEFORE_HIDE);

    PdfUtil* getCurrentlySelectedDocument();

};

#endif // MAINWINDOW_H
