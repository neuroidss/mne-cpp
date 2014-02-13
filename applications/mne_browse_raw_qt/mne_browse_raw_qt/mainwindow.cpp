//=============================================================================================================
/**
* @file     mainwindow.cpp
* @author   Florian Schlembach <florian.schlembach@tu-ilmenau.de>;
*           Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>;
*           Jens Haueisen <jens.haueisen@tu-ilmenau.de>
* @version  1.0
* @date     January, 2014
*
* @section  LICENSE
*
* Copyright (C) 2014, Florian Schlembach, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Implements the mainwindow function of mne_browse_raw_qt
*
*/

#include "mainwindow.h"

//Eigen
#include <Eigen/Core>
#include <Eigen/SparseCore>

//*************************************************************************************************************

using namespace MNE_BROWSE_RAW_QT;

//*************************************************************************************************************

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, m_qFileRaw("./MNE-sample-data/MEG/sample/sample_audvis_raw.fif")
, m_qSettings()
, m_rawSettings()
{
    //setup MVC
    setupModel();
    setupDelegate();
    setupView();
    setupLayout();

    createMenus();

    createLogDockWindow();
    setWindow();

}

//*************************************************************************************************************

MainWindow::~MainWindow()
{
}

//*************************************************************************************************************

void MainWindow::setupModel()
{
//    m_pRawModel = new RawModel(this);
    m_pRawModel = new RawModel(m_qFileRaw,this);
}

//*************************************************************************************************************

void MainWindow::setupDelegate()
{
    m_pRawDelegate = new RawDelegate(this);
}

//*************************************************************************************************************

void MainWindow::setupView()
{
    m_pTableView = new QTableView;

    m_pTableView->setModel(m_pRawModel); //set custom model
    m_pTableView->setItemDelegate(m_pRawDelegate); //set custom delegate

    //TableView settings
    setupViewSettings();

}

//*************************************************************************************************************

void MainWindow::setupLayout() {
    //set vertical layout
    QVBoxLayout *mainlayout = new QVBoxLayout;

    mainlayout->addWidget(m_pTableView);

    //set layouts
    QWidget *window = new QWidget();
    window->setLayout(mainlayout);

    setCentralWidget(window);
}

//*************************************************************************************************************

void MainWindow::setupViewSettings() {
    //set some size settings for m_pTableView
    m_pTableView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pTableView->setShowGrid(false);
    m_pTableView->horizontalHeader()->hide();
    m_pTableView->verticalHeader()->setDefaultSectionSize(m_pRawDelegate->m_dPlotHeight);

    m_pTableView->setAutoScroll(false);
    m_pTableView->setColumnHidden(0,true); //because content is plotted jointly with column=1

    m_pTableView->resizeColumnsToContents();

    m_pTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    //set context menu
    m_pTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_pTableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));

    //set position of QScrollArea
//    m_pTableView->horizontalScrollBar()->setValue(m_pRawModel->relFiffCursor()+m_pRawModel->m_iWindowSize/2);

    //activate kinetic scrolling
    QScroller::grabGesture(m_pTableView,QScroller::MiddleMouseButtonGesture);

    //connect QScrollBar with model in order to reload data samples
    connect(m_pTableView->horizontalScrollBar(),SIGNAL(valueChanged(int)),m_pRawModel,SLOT(reloadData(int)));
}

//*************************************************************************************************************

void MainWindow::createMenus() {
    QMenu *fileMenu = new QMenu(tr("&File"), this);

    QAction *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcuts(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    QAction *writeAction = fileMenu->addAction(tr("&Save As..."));
    openAction->setShortcuts(QKeySequence::SaveAs);
    connect(writeAction, SIGNAL(triggered()), this, SLOT(writeFile()));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcuts(QKeySequence::Quit);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    menuBar()->addMenu(fileMenu);
}

void MainWindow::setWindow() {
    setWindowTitle(CInfo::AppNameShort());
    resize(m_qSettings.value("MainWindow/size").toSize());
    this->move(50,50);
}

//=============================================================================================================
//Log

void MainWindow::createLogDockWindow()
{
    //Log TextBrowser
    m_pDockWidget_Log = new QDockWidget(tr("Log"), this);

    m_pTextBrowser_Log = new QTextBrowser(m_pDockWidget_Log);

    m_pDockWidget_Log->setWidget(m_pTextBrowser_Log);

    m_pDockWidget_Log->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_pDockWidget_Log);

    //Set standard LogLevel
    setLogLevel(_LogLvMax);
}

//*************************************************************************************************************

void MainWindow::writeToLog(const QString& logMsg, LogKind lgknd, LogLevel lglvl)
{
    if(lglvl<=m_eLogLevelCurrent) {
        if(lgknd == _LogKndError)
            m_pTextBrowser_Log->insertHtml("<font color=red><b>Error:</b> "+logMsg+"</font>");
        else if(lgknd == _LogKndWarning)
            m_pTextBrowser_Log->insertHtml("<font color=blue><b>Warning:</b> "+logMsg+"</font>");
        else
            m_pTextBrowser_Log->insertHtml(logMsg);
        m_pTextBrowser_Log->insertPlainText("\n"); // new line
        //scroll down to the latest entry
        QTextCursor c = m_pTextBrowser_Log->textCursor();
        c.movePosition(QTextCursor::End);
        m_pTextBrowser_Log->setTextCursor(c);

        m_pTextBrowser_Log->verticalScrollBar()->setValue(m_pTextBrowser_Log->verticalScrollBar()->maximum());
    }
}

//*************************************************************************************************************

void MainWindow::setLogLevel(LogLevel lvl)
{
    switch(lvl) {
    case _LogLvMin:
        writeToLog(tr("minimal log level set"), _LogKndMessage, _LogLvMin);
        break;
    case _LogLvNormal:
        writeToLog(tr("normal log level set"), _LogKndMessage, _LogLvMin);
        break;
    case _LogLvMax:
        writeToLog(tr("maximum log level set"), _LogKndMessage, _LogLvMin);
        break;
    }

    m_eLogLevelCurrent = lvl;
}

//=============================================================================================================
// SLOTS

void MainWindow::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this,QString("Open fiff data file"),QString("./MNE-sample-data/MEG/sample/"),tr("fif data files (*.fif)"));
    QFile t_fileRaw(filename);

    if(m_pRawModel->loadFiffData(t_fileRaw)) {
        qDebug() << "Fiff data file" << filename << "loaded.";
        setupViewSettings();
    }
    else
        qDebug("ERROR loading fiff data file %s",filename.toLatin1().data());
}

void MainWindow::writeFile()
{
    QString filename = QFileDialog::getSaveFileName(this,QString("Write fiff data file"),QString("./MNE-sample-data/MEG/sample/"),tr("fif data files (*.fif)"));
    QFile t_fileRaw(filename);

    if(!m_pRawModel->writeFiffData(t_fileRaw))
        qDebug() << "MainWindow: ERROR writing fiff data file" << t_fileRaw.fileName() << "!";

}

void MainWindow::customContextMenuRequested(QPoint pos)
{
    //obtain index where index was clicked
    QModelIndex index = m_pTableView->indexAt(pos);

    //create custom context menu and actions
    QMenu *menu = new QMenu(this);
    QAction* doMarkChBad = menu->addAction(tr("Mark as bad"));
    QAction* doMarkChGood = menu->addAction(tr("Mark as good"));
    QAction* doApplyHPFFilter = menu->addAction(tr("Apply HPF to selected channels"));
    QAction* doApplyLPFFilter = menu->addAction(tr("Apply LPF to selected channels"));
    QAction* doApplyHPFFilterAll = menu->addAction(tr("Apply HPF to all channels"));
    QAction* doApplyLPFFilterAll = menu->addAction(tr("Apply LPF to all channels"));
//    QAction* undoApplyHPFFilter = menu->addAction(tr("Undo HPF"));
//    QAction* undoApplyLPFFilter = menu->addAction(tr("Undo LPF"));
    QAction* undoApplyFilter = menu->addAction(tr("Undo all filtering to all channels"));

    //get selected items
    QModelIndexList selected = m_pTableView->selectionModel()->selectedIndexes();

    //connect actions
    //marking
    connect(doMarkChBad,&QAction::triggered, [=](){
        m_pRawModel->markChBad(selected,1);
    });
    connect(doMarkChGood,&QAction::triggered, [=](){
        m_pRawModel->markChBad(selected,0);
    });
    //filtering
    connect(doApplyHPFFilter,&QAction::triggered, [=](){
        FilterOperator (m_pRawModel->m_filterOperators[0])*; //ToDo: iterate through m_filterOperators and select certain filterOperator instance
        m_pRawModel->applyFilter(selected,(m_pRawModel->m_filterOperators[0])*);
    });
    connect(doApplyLPFFilter,&QAction::triggered, [=](){
//        m_pRawModel->applyFilter(selected,ParksMcClellan::TPassType::LPF);
    });
    connect(doApplyHPFFilterAll,&QAction::triggered, [=](){
//        m_pRawModel->applyFilter(QModelIndexList(),ParksMcClellan::TPassType::HPF);
    });
    connect(doApplyLPFFilterAll,&QAction::triggered, [=](){
//        m_pRawModel->applyFilter(QModelIndexList(),ParksMcClellan::TPassType::LPF);
    });
//    connect(undoApplyHPFFilter,&QAction::triggered, [=](){
//        m_pRawModel->undoFilter(selected,ParksMcClellan::TPassType::HPF);
//    });
//    connect(undoApplyLPFFilter,&QAction::triggered, [=](){
//        m_pRawModel->undoFilter(selected,ParksMcClellan::TPassType::LPF);
//    });
    connect(undoApplyFilter,&QAction::triggered, [=](){
        m_pRawModel->undoFilter();
    });

    //show context menu
    menu->popup(m_pTableView->viewport()->mapToGlobal(pos));
}
