#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRectF>
#include "../program/settings.h"
#include "../program/randomnumbers.h"
#include "landscapeeditdialog.h"
#include "../bug/organism.h"
#include <QDir>
#include <QMessageBox>
#include <QScrollBar>
#include <QDockWidget>
#include <QList>
#include <QFileDialog>
#include <QTextStream>
#include "singleorganismwidget.h"
#include "math.h"
#include <QLineEdit>
#include "settingsdialog.h"
#include "waitingdialog.h"
#include <QThread>
#include "../program/saverandloader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_resumeSimulationAfterSave(false),
    m_autosavePath(QDir::temp().path() + "/Bugraft-" + QString::number(QCoreApplication::applicationPid()) + ".bugr"),
    m_justSaved(false)
{
    ui->setupUi(this);

    g_mainWindow = this;

    g_settings = new Settings();
    g_randomNumbers = new RandomNumbers();
    g_zoom = 1.0;

    m_graphicsViewZoom = new GraphicsViewZoom(ui->graphicsView);
    g_landscape = new Landscape(&m_graphicsScene);
    m_graphicsScene.m_background = g_landscape->makePixmap();

    m_curvedTreeGraphicsScene = new CurvedTreeGraphicsScene(ui->graphicsView);
    m_squareTreeGraphicsScene = new SquareTreeGraphicsScene(ui->graphicsView);

    g_landscape->createOrganisms();
    g_history = new History();

    m_graphicsScene.setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    //Save the large font in a variable to be applied later to various UI elements.
    //This isn't done in the UI editor, as different platforms seem to have very different
    //base font sizes.
    QFont font;
    g_largeFont.setPointSize(font.pointSize() * 11 / 8);
    g_extraLargeFont.setPointSize(font.pointSize() * 7 / 4);

    ui->simulationHiddenLabel->setFont(g_extraLargeFont);
    ui->simulationHiddenLabel->setVisible(false);

    ui->zoomSpinBox->setMaximum(g_settings->maxZoom * 100.0);
    ui->zoomSpinBox->setMinimum(g_settings->minZoom * 100.0);


    m_graphicsScene.setSceneRect(QRectF(0, 0, g_settings->environmentWidth, g_settings->environmentHeight));
    ui->graphicsView->setScene(&m_graphicsScene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    ui->treeGraphicsView->m_squareTreeGraphicsScene = m_squareTreeGraphicsScene;
    ui->treeGraphicsView->m_curvedTreeGraphicsScene = m_curvedTreeGraphicsScene;
    m_squareTreeGraphicsScene->start(g_landscape->m_startingSpecies);
    m_curvedTreeGraphicsScene->start(g_landscape->m_startingSpecies);
    ui->treeGraphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    treeStyleChanged();

    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    m_historyOrganismWidget = new SingleOrganismWidget();
    ui->historyOrganismFrame->layout()->addWidget(m_historyOrganismWidget);

    m_landscapeHistoryWidget = new LandscapeHistoryWidget();
    ui->historyLandscapeFrame->layout()->addWidget(m_landscapeHistoryWidget);


    hideHistoryTip();
    ui->historyWidget->setMinimumSize(ui->historyWidget->sizeHint());
    showHistoryTip();


    showHideHistory(false);


    //Create the first history data point for time 0
    g_history->logHistory();

    //Make the tree/history region fairly narrow to begin with.
    QList<int> splitterSizes;
    splitterSizes.push_back(2000);
    splitterSizes.push_back(80);
    ui->splitter->setSizes(splitterSizes);

    //Set the initial tree scale using the slider default position
    treeVerticalScaleChanged();

    connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(simulationSpeedChanged()));
    connect(ui->zoomSpinBox, SIGNAL(valueChanged(double)), this, SLOT(zoomSpinBoxChanged()));
    connect(m_graphicsViewZoom, SIGNAL(zoomed()), this, SLOT(zoomedWithMouseWheel()));
    connect(ui->actionEdit_landscape, SIGNAL(triggered()), this, SLOT(openLandscapeEditDialog()));
    connect(ui->startStopSimulationButton, SIGNAL(clicked()), this, SLOT(startStopSimulationButtonPressed()));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetSimulation()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(advanceOneTick()));
    connect(ui->treeVerticalScaleSlider, SIGNAL(valueChanged(int)), this, SLOT(treeVerticalScaleChanged()));
    connect(ui->actionSave_phylogenetic_tree, SIGNAL(triggered()), this, SLOT(saveTreeToFile()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->viewHistoryButton, SIGNAL(toggled(bool)), this, SLOT(showHideHistory(bool)));
    connect(ui->treeStyleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(treeStyleChanged()));
    connect(ui->actionSimulation_settings, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));
    connect(ui->actionSave_simulation, SIGNAL(triggered()), this, SLOT(saveSimulationManual()));
    connect(ui->actionLoad_simulation, SIGNAL(triggered()), this, SLOT(loadSimulationPrompt()));
}

MainWindow::~MainWindow()
{
    delete g_history;
    delete g_landscape;
    delete g_randomNumbers;
    delete g_settings;
    delete m_squareTreeGraphicsScene;
    delete m_curvedTreeGraphicsScene;
    delete ui;
}


void MainWindow::simulationSpeedChanged()
{
    if (simulationIsRunning())
    {
        //The simulation is hidden for maximum speed.
        //All slower speeds show the simulation in progress.
        if (ui->speedSlider->value() == 2)
            hideSimulation();
        else
            showSimulation();
    }

    switch (ui->speedSlider->value())
    {
    case 0:
        m_timer.setInterval(100);
        break;

    case 1:
    case 2:
        m_timer.setInterval(0);
        break;
    }
}

void MainWindow::zoomSpinBoxChanged()
{
    double zoomFactor = (ui->zoomSpinBox->value() / 100.0) / g_zoom;

    m_graphicsViewZoom->gentle_zoom(zoomFactor, false);
    zoomedWithMouseWheel();
}

void MainWindow::zoomedWithMouseWheel()
{
    ui->zoomSpinBox->blockSignals(true);
    double newSpinBoxValue = g_zoom * 100.0;
    ui->zoomSpinBox->setValue(newSpinBoxValue);
    ui->zoomSpinBox->blockSignals(false);
}

void MainWindow::openLandscapeEditDialog()
{
    bool simulationRunningAtFunctionStart = simulationIsRunning();
    stopSimulation();

    LandscapeEditDialog landscapeEditDialog(this);

    if (landscapeEditDialog.exec() == QDialog::Accepted)
    {
        landscapeEditDialog.setSettingsFromSpinBoxes();
        g_landscape->updateLandscape();
        m_graphicsScene.m_background = g_landscape->makePixmap();

        //If the simulation hasn't yet started, then we'll want to recreate
        //the first history log using the updated landscape.
        if (g_landscape->m_elapsedTime == 0)
        {
            g_history->reset();
            g_history->logHistory();
        }
    }

    if (simulationRunningAtFunctionStart)
        startSimulation();
}

void MainWindow::startStopSimulationButtonPressed()
{
    if (simulationIsRunning())
        stopSimulation();
    else
        startSimulation();
}


void MainWindow::startStopSimulation(bool start)
{
    //Start
    if (start)
    {
        if (ui->speedSlider->value() == 2)
            hideSimulation();
        ui->startStopSimulationButton->setText("Pause");
        ui->startStopSimulationLabel->setPixmap(QPixmap::fromImage(QImage(":/icons/pause-256.png")));
        m_timer.start();
    }

    //Stop
    else
    {
        if (ui->speedSlider->value() == 2)
            showSimulation();
        if (g_landscape->m_elapsedTime == 0)
            ui->startStopSimulationButton->setText("Start");
        else
            ui->startStopSimulationButton->setText("Resume");
        ui->startStopSimulationLabel->setPixmap(QPixmap::fromImage(QImage(":/icons/play-256.png")));
        m_timer.stop();
    }
}

void MainWindow::advanceOneTick()
{
    if (checkForExtinction())
        return;

    m_justSaved = false;

    (g_landscape->m_elapsedTime)++;
    updateTimeDisplay();

    g_landscape->advanceOneTick();
    g_landscape->setAllOrganismPositions();

    if (g_settings->simulationShown)
        ui->graphicsView->viewport()->update();


    //Update the landscape if the time is right and either the water or land
    //is set to change.
    if (g_landscape->m_elapsedTime % g_settings->landscapeChangeInterval == 0 &&
            (g_settings->landscapeShapeVariationRate > 0.0 ||
            (g_settings->waterLevelVariationMagnitude > 0.0 &&
            g_settings->waterLevelVariationRate > 0.0)))
    {
        g_settings->perlinZValue += g_settings->landscapeShapeVariationRate * g_settings->landscapeChangeVariationRateFactor;
        g_settings->waterLevelPerlinZValue += g_settings->waterLevelVariationRate * g_settings->waterLevelVariationRateFactor;
        g_landscape->updateLandscape();

        m_graphicsScene.m_background = g_landscape->makePixmap();
    }

    //Update the species definitions and update the trees
    if (g_landscape->m_elapsedTime % g_settings->speciesCheckInterval == 0)
    {
        g_landscape->checkSpecies();
        m_squareTreeGraphicsScene->growTreeOneStep(g_landscape->m_elapsedTime);
        m_curvedTreeGraphicsScene->growTreeOneStep(g_landscape->m_elapsedTime);
        updateCurrentTreeStyle();
        ui->treeGraphicsView->viewport()->update();
    }

    //Log the history
    if (g_landscape->m_elapsedTime % g_settings->historyLogInterval == 0)
        g_history->logHistory();
}

void MainWindow::resetSimulation()
{
    stopSimulation();

    if (g_landscape->populationIsNotExtinct() && g_landscape->m_elapsedTime > 0)
    {
        if (QMessageBox::No == QMessageBox::question(this, "Reset simulation?", "Are you sure you want to\n"
                                                           "reset the current simulation?\n\n"
                                                           "If you click 'Yes', all unsaved\n"
                                                           "progress will be lost."))
            return;
    }

    ui->startStopSimulationButton->setText("Start");

    m_graphicsScene.clear();
    g_landscape->reset();
    g_history->reset();

    m_squareTreeGraphicsScene->start(g_landscape->m_startingSpecies);
    m_curvedTreeGraphicsScene->start(g_landscape->m_startingSpecies);
    updateCurrentTreeStyle();

    m_graphicsScene.m_background = g_landscape->makePixmap();
    updateTimeDisplay();
    ui->graphicsView->viewport()->update();

    //Create the first history data point for time 0
    g_history->logHistory();
}

void MainWindow::updateTimeDisplay()
{
    QLocale addCommas(QLocale::English);
    QString elapedTimeText = addCommas.toString(g_landscape->m_elapsedTime);
    ui->timeLabel2->setText(elapedTimeText);
}

bool MainWindow::checkForExtinction()
{
    if (g_landscape->populationIsExtinct())
    {
        stopSimulation();
        QMessageBox::information(this, "Extinction!", "All animals have died and the\npopulation is now extinct.\n\n"
                                                      "You must reset or load a\nsimulation to continue.");
        return true;
    }
    return false;
}


void MainWindow::saveTreeToFile()
{
    bool simulationRunningAtFunctionStart = simulationIsRunning();
    stopSimulation();

    QString defaultFileNameAndPath = QDir::homePath() + "/tree.nwk";
    QString fullFileName = QFileDialog::getSaveFileName(this, "Save phylogenetic tree", defaultFileNameAndPath, "Newick (*.nwk)");

    if (fullFileName != "") //User did not hit cancel
    {
        QFile file(fullFileName);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);

        out << g_landscape->getNewickTree();
    }

    if (simulationRunningAtFunctionStart)
        startSimulation();
}


void MainWindow::treeVerticalScaleChanged()
{
    g_settings->treeStepHeight = 0.2 * pow(1.0116, ui->treeVerticalScaleSlider->value()) - 0.1;
    updateCurrentTreeStyle();
}


void MainWindow::closeEvent(QCloseEvent * event)
{
    bool simulationRunningAtFunctionStart = simulationIsRunning();
    stopSimulation();

    if (g_landscape->m_elapsedTime == 0 ||
            g_landscape->populationIsExtinct() ||
            m_justSaved)
        event->accept();

    else if (QMessageBox::No == QMessageBox::question(this, "Close program?",
                                                      "Are you sure you want to exit\n"
                                                      "the program?\n\n"
                                                      "If you click 'Yes', all unsaved\n"
                                                      "progress will be lost."))
    {
        event->ignore();
        if (simulationRunningAtFunctionStart)
            startSimulation();
    }

    else
        event->accept();
}



void MainWindow::showSimulation()
{
    if (!g_settings->simulationShown)
        g_landscape->setAllOrganismPositions();
    g_settings->simulationShown = true;

    ui->simulationHiddenLabel->setVisible(false);
    ui->graphicsView->setVisible(true);
    ui->zoomWidget->setEnabled(true);
}

void MainWindow::hideSimulation()
{
    g_settings->simulationShown = false;
    ui->graphicsView->setVisible(false);
    ui->simulationHiddenLabel->setVisible(true);
    ui->zoomSpinBox->findChild<QLineEdit*>()->deselect();  //On Windows, this wasn't deselecting on its own after using the mouse wheel, and it looked weird to have a disabled spin box with a selection.
    ui->zoomWidget->setEnabled(false);
}


void MainWindow::showHideHistory(bool show)
{
    ui->historyWidget->setVisible(show);
}


void MainWindow::setHistorySpecies(int species)
{
    ui->historySpeciesLabel->setText("Species: " + this->locale().toString(species));
}

void MainWindow::setHistoryDate(long long date)
{
    ui->historyDateLabel->setText("Date: " + this->locale().toString(date));
}

void MainWindow::clearHistorySpecies()
{
    ui->historySpeciesLabel->setText("Species: ");
}

void MainWindow::clearHistoryDate()
{
    ui->historyDateLabel->setText("Date: ");
}


void MainWindow::showHistoryTip()
{
    ui->historyTipLabel->setVisible(true);
    ui->landscapeHistoryWidget->setVisible(false);
    ui->speciesHistoryWidget->setVisible(false);
}
void MainWindow::hideHistoryTip()
{
    ui->historyTipLabel->setVisible(false);
    ui->landscapeHistoryWidget->setVisible(true);
    ui->speciesHistoryWidget->setVisible(true);
}



void MainWindow::treeStyleChanged()
{
    ui->treeGraphicsView->sendWidthToScene();
    updateCurrentTreeStyle();

    if (ui->treeStyleComboBox->currentIndex() == 0) //Square tree
        ui->treeGraphicsView->setScene(m_squareTreeGraphicsScene);
    else //Curved tree
        ui->treeGraphicsView->setScene(m_curvedTreeGraphicsScene);
}

void MainWindow::updateCurrentTreeStyle()
{
    if (ui->treeStyleComboBox->currentIndex() == 0) //Square tree
        m_squareTreeGraphicsScene->updateTree();
    else //Curved tree
        m_curvedTreeGraphicsScene->updateTree();
}



void MainWindow::openSettingsDialog()
{
    bool simulationRunningAtFunctionStart = simulationIsRunning();
    stopSimulation();

    SettingsDialog settingsDialog(this);
    settingsDialog.setWidgetsFromSettings();

    Settings settingsBefore = *g_settings;

    if (settingsDialog.exec()) //The user clicked OK
    {
        settingsDialog.setSettingsFromWidgets();

        if (settingsBefore.meanOrganismLifespan != g_settings->meanOrganismLifespan)
            g_randomNumbers->remakeOrganismLifespanGenerator();
    }

    if (simulationRunningAtFunctionStart)
        startSimulation();
}




void MainWindow::saveSimulationManual()
{
    m_resumeSimulationAfterSave = simulationIsRunning();
    stopSimulation();

    QString defaultSaveFileName = "bugraft_simulation";
    defaultSaveFileName += ".bugr";
    QString defaultSaveFileNameAndPath = g_settings->rememberedPath + "/" + defaultSaveFileName;

    QString fullFileName = QFileDialog::getSaveFileName(this, "Save simulation", defaultSaveFileNameAndPath, "Bugraft simulation (*.bugr)");

    if (fullFileName != "") //User did not hit cancel
    {
        g_settings->rememberPath(fullFileName);
        saveSimulation(fullFileName, "Saving...");
    }
    else //User hit cancel
    {
        if (m_resumeSimulationAfterSave)
            startSimulation();
    }
}

void MainWindow::saveSimulationAutomatic()
{
    m_resumeSimulationAfterSave = true;
    stopSimulation();
    saveSimulation(m_autosavePath, "Autosaving...");
}

void MainWindow::saveSimulation(QString fullFileName, QString message)
{
    ui->controlsWidget->setEnabled(false);

    WaitingDialog * waitingDialog = new WaitingDialog(this, message, false);
    waitingDialog->setWindowModality(Qt::WindowModal);
    waitingDialog->show();

    QThread * thread = new QThread;
    SaverAndLoader * saverAndLoader = new SaverAndLoader(fullFileName);
    saverAndLoader->moveToThread(thread);

    connect(thread, SIGNAL(started()), saverAndLoader, SLOT(saveSimulation()));
    connect(saverAndLoader, SIGNAL(finishedSaving()), thread, SLOT(quit()));
    connect(saverAndLoader, SIGNAL(finishedSaving()), saverAndLoader, SLOT(deleteLater()));
    connect(saverAndLoader, SIGNAL(finishedSaving()), this, SLOT(finishedSaving()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), waitingDialog, SLOT(deleteLater()));

    thread->start();

    m_justSaved = true;
}

void MainWindow::loadSimulationPrompt()
{
    bool simulationRunningAtFunctionStart = simulationIsRunning();
    stopSimulation();

    QString fullFileName = QFileDialog::getOpenFileName(this, "Load simulation", g_settings->rememberedPath, "Bugraft simulation (*.bugr)");

    if (fullFileName != "") //User did not hit cancel
    {
        g_settings->rememberPath(fullFileName);
        try
        {
            loadSimulation(fullFileName);
        }
        catch (...)
        {
            QMessageBox::critical(this, "Error loading file", "An error occurred when loading the file.\n\n"
                                                              "The file could be corrupt or it could be of the wrong type.");
        }
    }
    else //Cancelled
    {
        if (simulationRunningAtFunctionStart)
            startSimulation();
    }
}

void MainWindow::loadSimulation(QString fullFileName)
{
    ui->timeLabel2->setText("0");
    ui->controlsWidget->setEnabled(false);

    m_graphicsScene.clear();
    g_history->m_historyPoints.clear();
    g_landscape->cleanUp();

    WaitingDialog * waitingDialog = new WaitingDialog(this, "Loading", true);
    waitingDialog->setWindowModality(Qt::WindowModal);
    waitingDialog->show();

    QThread * thread = new QThread;
    SaverAndLoader * saverAndLoader = new SaverAndLoader(fullFileName);
    saverAndLoader->moveToThread(thread);

    connect(thread, SIGNAL(started()), saverAndLoader, SLOT(loadSimulation()));
    connect(saverAndLoader, SIGNAL(finishedLoading()), thread, SLOT(quit()));
    connect(saverAndLoader, SIGNAL(finishedLoading()), saverAndLoader, SLOT(deleteLater()));
    connect(saverAndLoader, SIGNAL(finishedLoading()), this, SLOT(finishedLoading()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), waitingDialog, SLOT(deleteLater()));

    thread->start();
}

void MainWindow::finishedSaving()
{
    ui->controlsWidget->setEnabled(true);
    if (m_resumeSimulationAfterSave)
        startSimulation();
}

void MainWindow::finishedLoading()
{
    //These next four lines have to be here, in the main GUI thread,
    //not in SaverAndLoader::loadSimulation() as QWidget-related stuff
    //is NOT thread safe and it can cause crashes, especially on Windows.
    g_mainWindow->m_graphicsScene.m_background = g_landscape->makePixmap();
    g_landscape->addOrganismsAfterLoad();
    g_mainWindow->m_squareTreeGraphicsScene->rebuildTree();
    g_mainWindow->m_curvedTreeGraphicsScene->rebuildTree();

    ui->controlsWidget->setEnabled(true);
    m_justSaved = true;

    updateTimeDisplay();
}
