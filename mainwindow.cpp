#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pdfPageListModel = new QStringListModel(this);
    ui->pdfPagesList->setModel(pdfPageListModel);

    pdfPageRangesListModel = new PdfRangesItemModel(pageRanges, this);
    ui->newPagesList->setModel(pdfPageRangesListModel);

    ui->pdfPagesList->setDragEnabled(true);
    ui->pdfPagesList->setDragDropMode(QAbstractItemView::DragOnly);

    ui->newPagesList->setDragEnabled(true);
    ui->newPagesList->setDragDropMode(QAbstractItemView::DragDrop);
    ui->newPagesList->setDropIndicatorShown(true);
    ui->newPagesList->setDefaultDropAction(Qt::CopyAction);
    ui->newPagesList->setDragDropOverwriteMode(false);
    ui->newPagesList->setAcceptDrops(true);

    // Set int validators
    ui->singlePageTxt->setValidator( new QIntValidator );
    ui->multiplePagesTxt->setValidator( new QRegExpValidator(QRegExp("(\\d+,?)+"), ui->multiplePagesTxt) );
    ui->pageRangeLastTxt->setValidator( new QIntValidator );
    ui->pageRangeFirstTxt->setValidator( new QIntValidator );

    // context menus
    connect(ui->pdfPagesList, SIGNAL(customContextMenuRequested( const QPoint&)), this, SLOT(pdfPagesContextMenu(const QPoint&)));
    connect(ui->newPagesList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(pdfNewPagesContextMenu(const QPoint&)));

    progBar = new QProgressBar();
    progBar->setRange(0,100);
    progBar->setValue(0);
    progBar->setMaximumSize(170,19);
    progBar->setVisible(false);
    ui->statusBar->addPermanentWidget(progBar);
    defaultProgBarFormat = progBar->format();

    connect(ui->newPagesList->model(), SIGNAL(rowsMoved(const QModelIndex &, int , int , const QModelIndex &, int )), this, SLOT(pdfNewPagesModelRowsMoved(const QModelIndex &, int , int , const QModelIndex &, int )));
    connect(ui->newPagesList, SIGNAL(indexesMoved(const QModelIndexList &)), this, SLOT(pdfNewPagesListIndexesMoved(const QModelIndexList &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Load_PDF_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open PDF", "", "PDF File (*.pdf *.PDF)");
    if (!fileName.isEmpty() && !fileName.isNull())
    {
        try
        {
            startProgressBar(100);
            incrementProgressBar(50);
            auto pdfDoc = new PdfUtil(fileName);

            if(currentlyLoadedDocument != nullptr)
                delete currentlyLoadedDocument;

            currentlyLoadedDocument = pdfDoc;

            ui->statusBar->showMessage("PDF File Loaded. Total pages: "+QString::number(currentlyLoadedDocument->GetPageCount()));
            completeProgressBar();

            updatePdfPageList();
        } catch (const PdfException& exception)
        {
            completeProgressBar(true);
            QMessageBox::critical(this, "Error", "Error!\n"+exception.getMessage());
        }
    }
}

void MainWindow::setCurrentlyDisplayedPage(int pageNum)
{
    if(currentlyLoadedDocument != nullptr)
    {
        try
        {
            startProgressBar(100);
            auto page = currentlyLoadedDocument->GetPdfRenderedPage(pageNum);
            incrementProgressBar(33);
            if(displayedPage != nullptr)
                delete displayedPage;

            displayedPage = page;
            displayedImage = displayedPage->getImage();

            incrementProgressBar(33);

            grScene = new QGraphicsScene();
            pixItem.setPixmap(QPixmap::fromImage(*displayedImage));

            grScene->addItem(&pixItem);
            ui->pdfRenderingView->setScene(grScene);

            ui->pdfRenderingView->fitInView(&pixItem, Qt::KeepAspectRatio);
            completeProgressBar();
        } catch (const PdfException& exception) {
            completeProgressBar(true);
            QMessageBox::critical(this, "Error", "Error!\n"+exception.getMessage());
        }
    }
}

void MainWindow::updatePdfPageList()
{
    if(currentlyLoadedDocument != nullptr)
    {
        QStringList pdfPages;
        for(int i = 1; i <= currentlyLoadedDocument->GetPageCount(); i++)
        {
            pdfPages.append(QString::number(i));
        }

        pdfPageListModel->setStringList(pdfPages);
        ui->pdfPagesList->setCurrentlyLoadedDocument(currentlyLoadedDocument);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    ui->pdfRenderingView->fitInView(&pixItem, Qt::KeepAspectRatio);
}

void MainWindow::startProgressBar(int maxValue, int initialValue, bool showText)
{
    progBar->setStyleSheet(defaultProgBarFormat);
    progBar->setMaximum(maxValue);
    progBar->setValue(initialValue);
    progBar->setTextVisible(showText);
    progBar->setVisible(true);
}

void MainWindow::incrementProgressBar(int increment)
{
    progBar->setValue(progBar->value() + increment);
}

void MainWindow::completeProgressBar(bool error, int delay_ms)
{
    if(error)
        progBar->setStyleSheet(errorProgBarFormat);
    progBar->setValue(progBar->maximum());
    if(timer != nullptr) {
        delete timer;
        timer = nullptr;
    }

    timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(hideProgressBar()));
    timer->start(delay_ms);
}

void MainWindow::on_pdfPagesList_doubleClicked(const QModelIndex &index)
{

}

void MainWindow::on_pdfPagesList_activated(const QModelIndex &index)
{

}

void MainWindow::on_pdfPagesList_clicked(const QModelIndex &index)
{
    int page = index.row();
    setCurrentlyDisplayedPage(page);
}

void MainWindow::pdfPagesContextMenu(const QPoint &point)
{
    QPoint globalpos = ui->pdfPagesList->mapToGlobal(point);

    if(ui->pdfPagesList->selectionModel()->selectedIndexes().length() > 1)
    {
        QMenu selectionMenu;
        QAction* action_addSelection = selectionMenu.addAction("&Add selection");
        if(selectionMenu.exec(globalpos) == action_addSelection)
        {
            // Add selection to list
            auto selection = ui->pdfPagesList->selectionModel()->selectedIndexes();
            if(selection.length()>1)
            {
                std::sort(selection.begin(), selection.end(), std::less<QModelIndex>());
                auto newItem = new PdfPageContinuousIntervalSpecificator(selection.first().row(), selection.last().row());
                //pageRanges.append(newItem);
                if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
                {
                    auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                    pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                    //pdfPageRangesListModel->setData(index, newItem->getDisplayText());
                }
            }
        }
    } else {
        auto pageNum = ui->pdfPagesList->indexAt(point);
        if(pageNum.isValid())
        {
            QMenu selectionMenu;
            QAction* action_addSelection = selectionMenu.addAction("&Add");
            if(selectionMenu.exec(globalpos) == action_addSelection)
            {
                // Add item to list
                auto newItem = new PdfSinglePageSpecificator(pageNum.row());
                //pageRanges.append(newItem);
                if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
                {
                    auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                    pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                    //pdfPageRangesListModel->setData(index, newItem->getDisplayText());
                }
            }
        } else {
            QMenu selectionMenu;
            QAction* action_addAll = selectionMenu.addAction("&Add All");
            if(selectionMenu.exec(globalpos) == action_addAll && pdfPageListModel->rowCount() > 0)
            {
                // Add all
                auto newItem = new PdfPageContinuousIntervalSpecificator(0, pdfPageListModel->rowCount()-1);
                pageRanges.append(newItem);
                if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
                {
                    auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                    pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                    //pdfPageRangesListModel->setData(index, newItem->getDisplayText());
                }
            }
        }
    }
}

void MainWindow::pdfNewPagesContextMenu(const QPoint &point)
{
    QPoint globalpos = ui->newPagesList->mapToGlobal(point);

    if(ui->newPagesList->selectionModel()->selectedIndexes().length() > 1)
    {
        QMenu selectionMenu;
        QAction* action_deleteSelection = selectionMenu.addAction("&Delete selection");
        if(selectionMenu.exec(globalpos) == action_deleteSelection)
        {
            // Remove selection
            auto selection = ui->newPagesList->selectionModel()->selectedIndexes();
            if(selection.length()>1)
            {
                std::sort(selection.begin(), selection.end(), std::less<QModelIndex>());
                pdfPageRangesListModel->removeRows(selection.first().row(), selection.length());

                for(int i = selection.length()-1; i >= 0 ; i--)
                    pageRanges.removeAt(selection.at(i).row());
            }
        }
    } else {
        auto pageNum = ui->newPagesList->indexAt(point);
        if(pageNum.isValid())
        {
            QMenu selectionMenu;
            QAction* action_delete = selectionMenu.addAction("&Delete");
            if(selectionMenu.exec(globalpos) == action_delete)
            {
                // Remove item
                pdfPageRangesListModel->removeRow(pageNum.row());
                pageRanges.removeAt(pageNum.row());
            }
        } else {
            QMenu selectionMenu;
            QAction* action_deleteAll = selectionMenu.addAction("&Delete All");
            if(selectionMenu.exec(globalpos) == action_deleteAll && pdfPageRangesListModel->rowCount() > 0)
            {
                // Remove all
                pdfPageRangesListModel->removeRows(0, pdfPageRangesListModel->rowCount());
                pageRanges.clear();
            }
        }
    }
}

void MainWindow::on_addSinglePageBtn_clicked()
{
    if(currentlyLoadedDocument != nullptr)
    {
        bool ok = false;
        int pageValue = ui->singlePageTxt->text().toInt(&ok);
        if(ok && pageValue >= 1 && pageValue <= currentlyLoadedDocument->GetPageCount())
        {
            auto newItem = new PdfSinglePageSpecificator(pageValue-1);
            pageRanges.append(newItem);
            if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
            {
                auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                pdfPageRangesListModel->setData(index, newItem->getDisplayText());
            }
        }
    }
}

void MainWindow::on_addPageRangeBtn_clicked()
{
    if(currentlyLoadedDocument != nullptr)
    {
        bool ok = false;
        int firstPageValue = ui->pageRangeFirstTxt->text().toInt(&ok);
        int lastPageValue = ui->pageRangeLastTxt->text().toInt(&ok);
        if(ok && firstPageValue >= 1 && firstPageValue <= currentlyLoadedDocument->GetPageCount() && lastPageValue >= 1 && lastPageValue <= currentlyLoadedDocument->GetPageCount())
        {
            if(firstPageValue > lastPageValue)
                std::swap(firstPageValue, lastPageValue);

            auto newItem = new PdfPageContinuousIntervalSpecificator(firstPageValue-1, lastPageValue-1);
            pageRanges.append(newItem);
            if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
            {
                auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                pdfPageRangesListModel->setData(index, newItem->getDisplayText());
            }
        }
    }
}

void MainWindow::on_addMultiplePagesBtn_clicked()
{
    if(currentlyLoadedDocument != nullptr)
    {
        auto newItem = new PdfMultiplePagesSpecificator(ui->multiplePagesTxt->text());
        if(newItem->getAllPages().length()>0)
        {
            int pagesCount = currentlyLoadedDocument->GetPageCount();
            for(auto page : newItem->getAllPages())
            {
                if(page < 0 || page >= pagesCount)
                    return;
            }
            pageRanges.append(newItem);
            if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
            {
                auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                pdfPageRangesListModel->setData(index, newItem->getDisplayText());
            }
        }
    }
}

void MainWindow::on_action_Export_PDF_triggered()
{
    if(currentlyLoadedDocument != nullptr && pageRanges.length() > 0)
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Output file", "", "PDF File (*.pdf)");
        if(!fileName.isEmpty() && !fileName.isNull())
        {
            try {
                int pagesCount = 0;

                for(auto pageRange : pageRanges)
                    pagesCount += pageRange->getAllPages().length();

                startProgressBar(pagesCount);
                PdfUtil::PdfNewDocument newDoc(currentlyLoadedDocument, fileName);

                for(auto pageRange : pageRanges)
                {
                    for(int page : pageRange->getAllPages())
                    {
                        newDoc.addPageFromParent(page);
                        incrementProgressBar();
                    }
                }

                newDoc.Save();

                ui->statusBar->showMessage("PDF file exported successfully to: "+fileName);
                completeProgressBar();
            } catch (const PdfException& exception) {
                completeProgressBar(true);
                QMessageBox::critical(this, "Error", "Error!\n"+exception.getMessage());
            }
        }
    }
}

void MainWindow::hideProgressBar()
{
    progBar->setVisible(false);
}

void MainWindow::on_action_Add_all_triggered()
{
    if(currentlyLoadedDocument != nullptr && currentlyLoadedDocument->GetPageCount() > 0)
    {
        auto newItem = new PdfPageContinuousIntervalSpecificator(0, pdfPageListModel->rowCount()-1);
        pageRanges.append(newItem);
        if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
        {
            auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
            pdfPageRangesListModel->setData(index, newItem->getDisplayText());
        }
    }
}

void MainWindow::on_action_Delete_all_triggered()
{
    if(pageRanges.length() > 0)
    {
        pdfPageRangesListModel->removeRows(0, pdfPageRangesListModel->rowCount());
        pageRanges.clear();
    }
}

void MainWindow::on_action_Exit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_action_About_triggered()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::pdfNewPagesListIndexesMoved(const QModelIndexList &indexes)
{
    pageRanges.move(indexes.first().row(), indexes.last().row());
}

void MainWindow::pdfNewPagesModelRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    pageRanges.move(start, end);
}
