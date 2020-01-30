#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pdfPageListModel.reset(new OpenedDocumentsTreeModel(loadedDocuments, this));
    ui->pdfPagesList->setModel(pdfPageListModel.data());

    ui->pdfPagesList->setCurrentlyLoadedDocuments(loadedDocuments);
    ui->pdfPagesList->setHeaderHidden(true);

    pdfPageRangesListModel.reset(new PdfRangesItemModel(pageRanges, this));
    ui->newPagesList->setModel(pdfPageRangesListModel.data());
    connect(pdfPageRangesListModel.data(), SIGNAL(itemDropped()), this, SLOT(newPagesItemDropped()));

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

    progBar.reset(new QProgressBar());
    progBar->setRange(0,100);
    progBar->setValue(0);
    progBar->setMaximumSize(170,19);
    progBar->setVisible(false);
    ui->statusBar->addPermanentWidget(progBar.data());
    defaultProgBarFormat = progBar->format();

    connect(ui->pdfPagesList, SIGNAL(navigationArrowPressed(int)), this, SLOT(pdfPagesArrowReceived(int )));

    connect(ui->newPagesList->model(), SIGNAL(rowsMoved(const QModelIndex &, int , int , const QModelIndex &, int )), this, SLOT(pdfNewPagesModelRowsMoved(const QModelIndex &, int , int , const QModelIndex &, int )));
    connect(ui->newPagesList, SIGNAL(indexesMoved(const QModelIndexList &)), this, SLOT(pdfNewPagesListIndexesMoved(const QModelIndexList &)));

    //Connect line edits to buttons
    connect(ui->singlePageTxt, SIGNAL(returnPressed()), ui->addSinglePageBtn, SIGNAL(clicked()));
    connect(ui->pageRangeLastTxt, SIGNAL(returnPressed()), ui->addPageRangeBtn, SIGNAL(clicked()));
    connect(ui->pageRangeFirstTxt, SIGNAL(returnPressed()), ui->addPageRangeBtn, SIGNAL(clicked()));
    connect(ui->multiplePagesTxt, SIGNAL(returnPressed()), ui->addMultiplePagesBtn, SIGNAL(clicked()));

    pdfDocsHint.reset(new HintWidget(tr("1. Drop a PDF here to open it")));
    pdfPagesHint.reset(new HintWidget(tr("3. Drop selected document pages or image files here")));
    pdfRenderHint.reset(new HintWidget(tr("2. Click on an open document to show a preview of its pages")));

    QFont hintFont;
    hintFont.setBold(true);
    hintFont.setPointSize(SETTINGS::HINTFONT_SIZE);

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

    ui->pdfPagesList->installEventFilter(new HintWidgetFactoryFilter(pdfDocsHint.data()));
    ui->newPagesList->installEventFilter(new HintWidgetFactoryFilter(pdfPagesHint.data()));
    ui->pdfRenderingView->installEventFilter(new HintWidgetFactoryFilter(pdfRenderHint.data()));

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    qDeleteAll(pageRanges);



    qDeleteAll(loadedDocuments);

    delete ui;
}

void MainWindow::on_action_Load_PDF_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PDF"), "", tr("PDF File") + " (*.pdf *.PDF)");
    if (!fileName.isEmpty() && !fileName.isNull())
    {
        try
        {
            startProgressBar(100);
            incrementProgressBar(50);
            auto pdfDoc = new PdfUtil(fileName);

            loadedDocuments.append(pdfDoc);

            ui->statusBar->showMessage(tr("PDF File Loaded. Total pages: %1").arg(QString::number(pdfDoc->GetPageCount())));
            completeProgressBar();

            addPdfPageList(loadedDocuments.last());

            pdfDocsHint->setVisible(false);

        } catch (const PdfException& exception)
        {
            completeProgressBar(true);
            QMessageBox::critical(this, tr("Error"), tr("Error!\n%1").arg(exception.getMessage()));
        }
    }
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

    timer.reset(new QTimer());
    timer->setSingleShot(true);
    connect(timer.data(), SIGNAL(timeout()), this, SLOT(hideProgressBar()));
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
            ui->statusBar->showMessage(tr("Before adding pages please select a document by clicking on one of its pages."));
            completeProgressBar(true);
        }
    }
    return nullptr;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls() && !event->mimeData()->hasFormat(SETTINGS::PDFPAGERANGESPECIFICATOR_P_MIME_TYPE))
    {
        bool hasPdf = true;
        auto urls = event->mimeData()->urls();
        for(auto url : urls)
        {
            QFileInfo fInfo(url.toLocalFile());
            if(fInfo.suffix().toLower() != "pdf")
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

    try
    {
        int pdfFiles = 0;
        for(auto url : urls)
        {
            QFileInfo fInfo(url.toLocalFile());
            if(fInfo.suffix().toLower() == "pdf")
            {
                    auto pdfDoc = new PdfUtil(url.toLocalFile());

                    loadedDocuments.append(pdfDoc);

                    addPdfPageList(loadedDocuments.last());

                    pdfFiles++;
            }
        }

        ui->statusBar->showMessage(tr("%1 PDF files loaded.").arg(QString::number(pdfFiles)));
        pdfDocsHint->setVisible(false);
    } catch (const PdfException& exception)
    {
        completeProgressBar(true);
        QMessageBox::critical(this, tr("Error"), tr("Error!\n%1").arg(exception.getMessage()));
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

}

void MainWindow::pdfPagesContextMenu(const QPoint &point)
{
    QPoint globalpos = ui->pdfPagesList->mapToGlobal(point);

    if(ui->pdfPagesList->selectionModel()->selectedIndexes().length() > 1)
    {
        QMenu selectionMenu;
        QAction* action_addSelection = selectionMenu.addAction(tr("&Add selection"));
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
                QAction* action_addSelection = selectionMenu.addAction(tr("&Add"));
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
                QAction* action_addSelection = selectionMenu.addAction(tr("&Add whole document"));
                QAction* action_deleteSelection = selectionMenu.addAction(tr("&Remove document and added pages"));
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
        } else { // Right click on the empty portion of the list


        }
    }
}

void MainWindow::pdfNewPagesContextMenu(const QPoint &point)
{
    QPoint globalpos = ui->newPagesList->mapToGlobal(point);

    if(ui->newPagesList->selectionModel()->selectedIndexes().length() > 1)
    {
        QMenu selectionMenu;
        QAction* action_deleteSelection = selectionMenu.addAction(tr("&Remove selection"));
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
            QAction* action_delete = selectionMenu.addAction(tr("&Remove"));
            if(selectionMenu.exec(globalpos) == action_delete)
            {
                // Remove item
                pdfPageRangesListModel->removeRow(pageNum.row());
            }
        } else {
            QMenu selectionMenu;
            QAction* action_deleteAll = selectionMenu.addAction(tr("&Remove All"));
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
    if(pageRanges.length() > 0)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Output file"), "", tr("PDF File") + " (*.pdf)");
        if(!fileName.isEmpty() && !fileName.isNull())
        {
            try {
                int pagesCount = 0;

                pagesCount = pageRanges.length();

                startProgressBar(pagesCount);
                PdfUtil::PdfNewDocument newDoc(fileName);

                for(auto pageRange : pageRanges)
                {
                    if(pageRange->isImage())
                        newDoc.addPageFromImage(pageRange->getDocumentPath());
                    else
                        newDoc.addPagesFromRange(pageRange);
                    incrementProgressBar();
                }

                newDoc.Save();

                ui->statusBar->showMessage(tr("PDF file exported successfully to: %1").arg(fileName));
                completeProgressBar();
            } catch (const PdfException& exception) {
                completeProgressBar(true);
                QMessageBox::critical(this, tr("Error"), tr("Error!\n%1").arg(exception.getMessage()));
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
        }
    }
}

void MainWindow::newPagesItemDropped()
{
    pdfPagesHint->setVisible(false);
}

void MainWindow::on_actionAdd_from_image_file_triggered()
{
    QStringList imgFiles = QFileDialog::getOpenFileNames(this, tr("Add image file"), "", tr("Image file") + " (*.jpg *.jpeg *.png *.gif *.bmp)");
    if(!imgFiles.isEmpty())
    {
        int rCount = pdfPageRangesListModel->rowCount();
        pdfPageRangesListModel->insertRows(rCount, imgFiles.size());

        for(int i = 0; i < imgFiles.size(); i++)
        {
            const QString& imgF = imgFiles.at(i);
            auto t_item = pdfPageRangesListModel->index(rCount + i);
            pdfPageRangesListModel->setData(t_item, QVariant::fromValue<PdfPageRangeSpecificator*>(reinterpret_cast<PdfPageRangeSpecificator*>(new PdfImagePageSpecificator(imgF))));
        }

        emit pdfPageRangesListModel->itemDropped();
    }
}
