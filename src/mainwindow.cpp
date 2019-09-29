#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pdfPageListModel = new OpenedDocumentsTreeModel(loadedDocuments, this);
    ui->pdfPagesList->setModel(pdfPageListModel);

    ui->pdfPagesList->setCurrentlyLoadedDocuments(loadedDocuments);
    ui->pdfPagesList->setHeaderHidden(true);

    pdfPageRangesListModel = new PdfRangesItemModel(pageRanges, this);
    ui->newPagesList->setModel(pdfPageRangesListModel);
    connect(pdfPageRangesListModel, SIGNAL(itemDropped()), this, SLOT(newPagesItemDropped()));

    ui->pdfPagesList->setDragEnabled(true);
    ui->pdfPagesList->setDragDropMode(QAbstractItemView::DragOnly);

    ui->newPagesList->setDragEnabled(true);
    ui->newPagesList->setDragDropMode(QAbstractItemView::DragDrop);
    ui->newPagesList->setDropIndicatorShown(true);
    ui->newPagesList->setDefaultDropAction(Qt::CopyAction);
    ui->newPagesList->setDragDropOverwriteMode(false);
    ui->newPagesList->setAcceptDrops(true);

    QToolTipper* toolTipper = new QToolTipper();
    ui->newPagesList->installEventFilter(toolTipper);

    HTMLDelegate* htmlDelegate = new HTMLDelegate();
    ui->newPagesList->setItemDelegate(htmlDelegate);

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

    //pixItem.setTransformationMode(Qt::SmoothTransformation);
    //ui->pdfRenderingView->setViewport(new QGLWidget);

    connect(ui->pdfPagesList, SIGNAL(navigationArrowPressed(int)), this, SLOT(pdfPagesArrowReceived(int )));

    /*QPixmap blankPixmap(500,500);
    blankPixmap.fill(Qt::white);
    pixItem.setPixmap(blankPixmap);

    grScene = new QGraphicsScene();
    grScene->addItem(&pixItem);
    ui->pdfRenderingView->setScene(grScene);*/

    connect(ui->newPagesList->model(), SIGNAL(rowsMoved(const QModelIndex &, int , int , const QModelIndex &, int )), this, SLOT(pdfNewPagesModelRowsMoved(const QModelIndex &, int , int , const QModelIndex &, int )));
    connect(ui->newPagesList, SIGNAL(indexesMoved(const QModelIndexList &)), this, SLOT(pdfNewPagesListIndexesMoved(const QModelIndexList &)));

    //Connect line edits to buttons
    connect(ui->singlePageTxt, SIGNAL(returnPressed()), ui->addSinglePageBtn, SIGNAL(clicked()));
    connect(ui->pageRangeLastTxt, SIGNAL(returnPressed()), ui->addPageRangeBtn, SIGNAL(clicked()));
    connect(ui->pageRangeFirstTxt, SIGNAL(returnPressed()), ui->addPageRangeBtn, SIGNAL(clicked()));
    connect(ui->multiplePagesTxt, SIGNAL(returnPressed()), ui->addMultiplePagesBtn, SIGNAL(clicked()));

    pdfDocsHint = new HintWidget("Drop a PDF here to open it");
    pdfPagesHint = new HintWidget("Drop pages here or add them through the left controls");
    pdfRenderHint = new HintWidget("Click on an open document to show a preview of its pages");

    QFont hintFont;
    hintFont.setBold(true);
    hintFont.setPointSize(10);

    pdfDocsHint->setFont(hintFont);
    pdfPagesHint->setFont(hintFont);
    pdfRenderHint->setFont(hintFont);

    pdfRenderHint->setColor(QColor::fromRgb(0xde, 0xde, 0xde));

    pdfDocsHint->setParent(ui->pdfPagesList);
    pdfDocsHint->resize(ui->pdfPagesList->size());

    pdfPagesHint->setParent(ui->newPagesList);
    pdfPagesHint->resize(ui->newPagesList->size());

    pdfRenderHint->setParent(ui->pdfRenderingView);
    pdfRenderHint->resize(ui->pdfRenderingView->size());

    ui->pdfPagesList->installEventFilter(new HintWidgetFactoryFilter(pdfDocsHint));
    ui->newPagesList->installEventFilter(new HintWidgetFactoryFilter(pdfPagesHint));
    ui->pdfRenderingView->installEventFilter(new HintWidgetFactoryFilter(pdfRenderHint));

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    qDeleteAll(pageRanges);

    if(displayedImage != nullptr)
        delete displayedImage;

    if(displayedPage != nullptr)
        delete displayedPage;

    qDeleteAll(loadedDocuments);

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

            /*if(currentlyLoadedDocument != nullptr)
                delete currentlyLoadedDocument;

            currentlyLoadedDocument = pdfDoc;*/
            loadedDocuments.append(pdfDoc);

            ui->statusBar->showMessage("PDF File Loaded. Total pages: "+QString::number(pdfDoc->GetPageCount()));
            completeProgressBar();

            addPdfPageList(loadedDocuments.last());

            pdfDocsHint->setVisible(false);
            /*if(pageRanges.size() > 0)
                pdfPageRangesListModel->removeRows(0, pdfPageListModel->rowCount());*/
        } catch (const PdfException& exception)
        {
            completeProgressBar(true);
            QMessageBox::critical(this, "Error", "Error!\n"+exception.getMessage());
        }
    }
}

void MainWindow::setCurrentlyDisplayedPage(int pageNum, int docIndex)
{
    auto currentlyLoadedDocument = loadedDocuments.at(docIndex);
    if(currentlyLoadedDocument != nullptr)
    {
        try
        {
            /*startProgressBar(100);
            auto page = currentlyLoadedDocument->GetPdfRenderedPage(pageNum);
            incrementProgressBar(33);
            if(displayedPage != nullptr)
                delete displayedPage;

            displayedPage = page;
            displayedImage = displayedPage->getImage();

            incrementProgressBar(33);

            ui->pdfRenderingView->setScene(nullptr);

            grScene = new QGraphicsScene();
            pixItem.setPixmap(QPixmap::fromImage(*displayedImage));

            grScene->addItem(&pixItem);
            ui->pdfRenderingView->setScene(grScene);

            ui->pdfRenderingView->fitInView(&pixItem, Qt::KeepAspectRatio);
            completeProgressBar();*/
        } catch (const PdfException& exception) {
            completeProgressBar(true);
            QMessageBox::critical(this, "Error", "Error!\n"+exception.getMessage());
        }
    }
}

void MainWindow::updatePdfPageList()
{
    /*if(currentlyLoadedDocument != nullptr)
    {
        QStringList pdfPages;
        for(int i = 1; i <= currentlyLoadedDocument->GetPageCount(); i++)
        {
            pdfPages.append(QString::number(i));
        }

        //pdfPageListModel->setStringList(pdfPages);
        //ui->pdfPagesList->setCurrentlyLoadedDocument(currentlyLoadedDocument);

    }*/
}

void MainWindow::addPdfPageList(PdfUtil *doc)
{
    QStandardItem* newDoc = new QStandardItem(doc->GetDocName());
    newDoc->setColumnCount(1);
    newDoc->setDragEnabled(false);
    newDoc->setSelectable(false);
    newDoc->setDropEnabled(false);

    for(int i = 0; i < doc->GetPageCount(); i++)
    {
        QStandardItem* page = new QStandardItem(QString::number(i+1));
        page->setColumnCount(1);
        page->setDragEnabled(true);
        page->setSelectable(true);
        page->setDropEnabled(false);

        newDoc->appendRow(page);
    }

    pdfPageListModel->setItem(pdfPageListModel->rowCount(), 0, newDoc);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    //ui->pdfRenderingView->fitInView(&pixItem, Qt::KeepAspectRatio);
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

    if(!progBar->isVisible())
        progBar->setVisible(true);

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

PdfUtil *MainWindow::getCurrentlySelectedDocument()
{
    if(loadedDocuments.size() > 0)
    {
        auto selection = ui->pdfPagesList->selectionModel()->selectedIndexes();
        if(selection.size() > 0)
        {
            auto index = selection.first();
            int docInd = 0;
            if(index.parent().isValid())
                docInd = index.parent().row();
            else
                docInd = index.row();
            if(docInd >= 0 && docInd < loadedDocuments.size())
                return loadedDocuments.at(docInd);
        } else {
            ui->statusBar->showMessage("Before adding pages please select a document by clicking on one of its pages.");
            completeProgressBar(true);
        }
    }
    return nullptr;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    bool hasPdf = true;
    if(event->mimeData()->hasUrls())
    {
        auto urls = event->mimeData()->urls();
        for(auto url : urls)
        {
            QFileInfo fInfo(url.toLocalFile());
            if(fInfo.suffix() != "pdf")
            {
                hasPdf = false;
                break;
            }
        }
        if(hasPdf)
            event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    auto urls = event->mimeData()->urls();
    qDebug() << event->mimeData()->data("text/uri-list");
    qDebug() << event->mimeData()->formats();

    try
    {
        int pdfFiles = 0;
        for(auto url : urls)
        {
            QFileInfo fInfo(url.toLocalFile());
            if(fInfo.suffix() == "pdf")
            {
                    auto pdfDoc = new PdfUtil(url.toLocalFile());

                    loadedDocuments.append(pdfDoc);

                    addPdfPageList(loadedDocuments.last());

                    pdfFiles++;
            }
        }

        ui->statusBar->showMessage(QString::number(pdfFiles)+" PDF files loaded.");
        pdfDocsHint->setVisible(false);
    } catch (const PdfException& exception)
    {
        completeProgressBar(true);
        QMessageBox::critical(this, "Error", "Error!\n"+exception.getMessage());
    }
}

void MainWindow::on_pdfPagesList_doubleClicked(const QModelIndex &index)
{

}

void MainWindow::on_pdfPagesList_activated(const QModelIndex &index)
{

}

void MainWindow::on_pdfPagesList_clicked(const QModelIndex &index)
{
    int page = 0, parent = 0;
    if(index.parent().isValid())
    {
        page = qvariant_cast<QString>(index.data()).toInt()-1;
        parent = index.parent().row();
        auto doc = ui->pdfRenderingView->getCurrentDoc();
        if(doc != loadedDocuments.at(parent))
            ui->pdfRenderingView->displayDocPages(loadedDocuments.at(parent));

        ui->pdfRenderingView->navigateToPage(page);

        pdfRenderHint->setVisible(false);
    } else {
        parent = index.row();
        ui->pdfRenderingView->displayDocPages(loadedDocuments.at(parent));
        pdfRenderHint->setVisible(false);
    }
    //setCurrentlyDisplayedPage(page, parent);

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
                auto newItem = new PdfPageContinuousIntervalSpecificator(getCurrentlySelectedDocument()->GetDocPath(), selection.first().row(), selection.last().row(), getCurrentlySelectedDocument());
                if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
                {
                    auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                    pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                    pdfPagesHint->setVisible(false);
                }
            }
        }
    } else {
        auto pageNum = ui->pdfPagesList->indexAt(point);
        if(pageNum.isValid())
        {
            if(pageNum.parent().isValid())
            {
                //Right click on page
                QMenu selectionMenu;
                QAction* action_addSelection = selectionMenu.addAction("&Add");
                if(selectionMenu.exec(globalpos) == action_addSelection)
                {
                    // Add item to list
                    auto newItem = new PdfSinglePageSpecificator(getCurrentlySelectedDocument()->GetDocPath(), pageNum.row(), getCurrentlySelectedDocument());
                    if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
                    {
                        auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                        pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                        pdfPagesHint->setVisible(false);
                    }
                }
            } else {
                //Right click on document
                QMenu selectionMenu;
                QAction* action_addSelection = selectionMenu.addAction("&Add whole document");
                QAction* action_deleteSelection = selectionMenu.addAction("&Remove document and added pages");
                auto res = selectionMenu.exec(globalpos);
                if(res == action_addSelection)
                {
                    // Add whole document to list
                    auto doc = loadedDocuments.at(pageNum.row());
                    int docPageCount = doc->GetPageCount();
                    auto newItem = new PdfPageContinuousIntervalSpecificator(doc->GetDocPath(), 0, docPageCount-1, doc);
                    if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
                    {
                        auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                        pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                        pdfPagesHint->setVisible(false);
                    }
                } else if (res == action_deleteSelection)
                {
                    // Remove document
                    auto doc = loadedDocuments.at(pageNum.row());

                    if(doc == ui->pdfRenderingView->getCurrentDoc())
                        ui->pdfRenderingView->removeAllWidgets();

                    loadedDocuments.removeAt(pageNum.row());
                    pdfPageListModel->removeRow(pageNum.row());
                    pdfPageRangesListModel->removeRowsOfDocument(doc);
                    delete doc;
                }
            }
        }/* else { // Dropped support to right click on the empty portion of the list
            QMenu selectionMenu;
            QAction* action_addAll = selectionMenu.addAction("&Add all from selected document");
            if(selectionMenu.exec(globalpos) == action_addAll && pdfPageListModel->rowCount() > 0)
            {
                auto doc = getCurrentlySelectedDocument();
                if (doc != nullptr)
                {
                    // Add all
                    auto newItem = new PdfPageContinuousIntervalSpecificator(doc->GetDocPath(), 0, pdfPageListModel->rowCount()-1, doc);
                    pageRanges.append(newItem);
                    if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
                    {
                        auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                        pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                    }
                }
            }
        }*/
    }
}

void MainWindow::pdfNewPagesContextMenu(const QPoint &point)
{
    QPoint globalpos = ui->newPagesList->mapToGlobal(point);

    if(ui->newPagesList->selectionModel()->selectedIndexes().length() > 1)
    {
        QMenu selectionMenu;
        QAction* action_deleteSelection = selectionMenu.addAction("&Remove selection");
        if(selectionMenu.exec(globalpos) == action_deleteSelection)
        {
            // Remove selection
            auto selection = ui->newPagesList->selectionModel()->selectedIndexes();
            if(selection.length()>1)
            {
                std::sort(selection.begin(), selection.end(), std::less<QModelIndex>());
                pdfPageRangesListModel->removeRows(selection.first().row(), selection.length());
            }
        }
    } else {
        auto pageNum = ui->newPagesList->indexAt(point);
        if(pageNum.isValid())
        {
            QMenu selectionMenu;
            QAction* action_delete = selectionMenu.addAction("&Remove");
            if(selectionMenu.exec(globalpos) == action_delete)
            {
                // Remove item
                pdfPageRangesListModel->removeRow(pageNum.row());
            }
        } else {
            QMenu selectionMenu;
            QAction* action_deleteAll = selectionMenu.addAction("&Remove All");
            if(selectionMenu.exec(globalpos) == action_deleteAll && pdfPageRangesListModel->rowCount() > 0)
            {
                // Remove all
                pdfPageRangesListModel->removeRows(0, pdfPageRangesListModel->rowCount());
            }
        }
    }
}

void MainWindow::on_addSinglePageBtn_clicked()
{
    PdfUtil* currentlyLoadedDocument = getCurrentlySelectedDocument();
    if(currentlyLoadedDocument != nullptr)
    {
        bool ok = false;
        int pageValue = ui->singlePageTxt->text().toInt(&ok);
        if(ok && pageValue >= 1 && pageValue <= currentlyLoadedDocument->GetPageCount())
        {
            auto newItem = new PdfSinglePageSpecificator(getCurrentlySelectedDocument()->GetDocPath(), pageValue-1, getCurrentlySelectedDocument());
            if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
            {
                auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                pdfPagesHint->setVisible(false);
            }
        }
    }
}

void MainWindow::on_addPageRangeBtn_clicked()
{
    PdfUtil* currentlyLoadedDocument = getCurrentlySelectedDocument();
    if(currentlyLoadedDocument != nullptr)
    {
        bool ok = false;
        int firstPageValue = ui->pageRangeFirstTxt->text().toInt(&ok);
        int lastPageValue = ui->pageRangeLastTxt->text().toInt(&ok);
        if(ok && firstPageValue >= 1 && firstPageValue <= currentlyLoadedDocument->GetPageCount() && lastPageValue >= 1 && lastPageValue <= currentlyLoadedDocument->GetPageCount())
        {
            if(firstPageValue > lastPageValue)
                std::swap(firstPageValue, lastPageValue);

            auto newItem = new PdfPageContinuousIntervalSpecificator(getCurrentlySelectedDocument()->GetDocPath(), firstPageValue-1, lastPageValue-1, getCurrentlySelectedDocument());
            if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
            {
                auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                pdfPagesHint->setVisible(false);
            }
        }
    }
}

void MainWindow::on_addMultiplePagesBtn_clicked()
{
    PdfUtil* currentlyLoadedDocument = getCurrentlySelectedDocument();
    if(currentlyLoadedDocument != nullptr)
    {
        auto newItem = new PdfMultiplePagesSpecificator(getCurrentlySelectedDocument()->GetDocPath(), ui->multiplePagesTxt->text(), getCurrentlySelectedDocument());
        if(newItem->getAllPages().length()>0)
        {
            int pagesCount = currentlyLoadedDocument->GetPageCount();
            for(auto page : newItem->getAllPages())
            {
                if(page < 0 || page >= pagesCount)
                    return;
            }
            if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
            {
                auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                pdfPagesHint->setVisible(false);
            }
        }
    }
}

void MainWindow::on_action_Export_PDF_triggered()
{
    if(loadedDocuments.size() > 0 && pageRanges.length() > 0)
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Output file", "", "PDF File (*.pdf)");
        if(!fileName.isEmpty() && !fileName.isNull())
        {
            try {
                int pagesCount = 0;

                pagesCount = pageRanges.length();

                startProgressBar(pagesCount);
                PdfUtil::PdfNewDocument newDoc(fileName);

                for(auto pageRange : pageRanges)
                {
                    newDoc.addPagesFromRange(pageRange);
                    incrementProgressBar();
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
    /*PdfUtil* currentlyLoadedDocument = getCurrentlySelectedDocument();
    if(currentlyLoadedDocument != nullptr && currentlyLoadedDocument->GetPageCount() > 0)
    {
        auto newItem = new PdfPageContinuousIntervalSpecificator(getCurrentlySelectedDocument()->GetDocPath(), 0, currentlyLoadedDocument->GetPageCount()-1, getCurrentlySelectedDocument());
        if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
        {
            auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
            pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
        }
    }*/

    // Add all pages from all documents
    for(auto doc : loadedDocuments)
    {
        if(doc != nullptr && doc->GetPageCount() > 0)
        {
            auto newItem = new PdfPageContinuousIntervalSpecificator(doc->GetDocPath(), 0, doc->GetPageCount()-1, doc);
            if(pdfPageRangesListModel->insertRow(pdfPageRangesListModel->rowCount()))
            {
                auto index = pdfPageRangesListModel->index(pdfPageRangesListModel->rowCount()-1);
                pdfPageRangesListModel->setData(index, QVariant::fromValue<PdfPageRangeSpecificator*>(newItem));
                pdfPagesHint->setVisible(false);
            }
        }
    }
}

void MainWindow::on_action_Delete_all_triggered()
{
    if(pageRanges.length() > 0)
    {
        pdfPageRangesListModel->removeRows(0, pdfPageRangesListModel->rowCount());
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

void MainWindow::pdfPagesArrowReceived(int key)
{
    int page = 0, parent = 0;

    auto indexes = ui->pdfPagesList->selectionModel()->selectedIndexes();

    if(indexes.size() == 1)
    {
        auto index = indexes.first();
        if(index.isValid())
        {
            if(index.parent().isValid())
            {
                page = qvariant_cast<QString>(index.data()).toInt()-1;
                parent = index.parent().row();
            } else {
                parent = index.row();
            }
            setCurrentlyDisplayedPage(page, parent);
        }
    }
    /*
    switch(key)
    {
    case Qt::Key_Down:
        if(currentlyDisplayedPageNum+1 >= 0 && currentlyDisplayedPageNum+1 < currentlyLoadedDocument->GetPageCount())
        {
            int page = 0, parent = 0;

            auto parentIndex = ui->pdfPagesList->selectionModel()->currentIndex().parent();
            if(parentIndex.parent().isValid())
            {
                page = currentlyDisplayedPageNum+1;
                parent = parentIndex.row();
            } else {
                parent = parentIndex.row();
            }
            setCurrentlyDisplayedPage(page, parent);
        }
        break;
    case Qt::Key_Up:
        if(currentlyDisplayedPageNum-1 >= 0 && currentlyDisplayedPageNum-1 < currentlyLoadedDocument->GetPageCount())
        {
            int page = 0;
            auto parent = ui->pdfPagesList->selectionModel()->currentIndex().parent();
            if(parent.isValid())
                page = currentlyDisplayedPageNum+1;
            setCurrentlyDisplayedPage(page, parent.row());
        }
        break;
    }*/
}

void MainWindow::newPagesItemDropped()
{
    pdfPagesHint->setVisible(false);
}
