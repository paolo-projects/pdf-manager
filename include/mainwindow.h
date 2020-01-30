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
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPointer>

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
#include "pagegridwidget.h"
#include "hintwidget.h"

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

    void newPagesItemDropped();

    void on_actionAdd_from_image_file_triggered();

private:
    Ui::MainWindow *ui;
    QList<PdfUtil*> loadedDocuments;
    QScopedPointer<QProgressBar> progBar;

    QScopedPointer<HintWidget> pdfDocsHint, pdfPagesHint, pdfRenderHint;

    QScopedPointer<OpenedDocumentsTreeModel> pdfPageListModel;

    QList<PdfPageRangeSpecificator*> pageRanges;
    QScopedPointer<PdfRangesItemModel> pdfPageRangesListModel;

    QScopedPointer<QTimer> timer;

    QString defaultProgBarFormat;
    QString errorProgBarFormat =  "QProgressBar {"
                                  "background-color: #000000;"
                                  "}"
                                  ""
                                  "QProgressBar::chunk {"
                                  "background-color: #FF0000;"
                                  "}";

    void addPdfPageList(PdfUtil* doc);
    void resizeEvent(QResizeEvent* event) override;

    void startProgressBar(int maxValue, int initialValue = 0, bool showText = SETTINGS::DEFAULT_PROGBAR_DISPLAY_TEXT);
    void incrementProgressBar(int increment = 1);
    void completeProgressBar(bool error = false, int delay_ms = SETTINGS::DEFAULT_PROGBAR_DELAY_BEFORE_HIDE);

    PdfUtil* getCurrentlySelectedDocument();


    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // MAINWINDOW_H
