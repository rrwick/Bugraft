#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene>
#include "../program/globals.h"
#include "environmentgraphicsscene.h"
#include "graphicsviewzoom.h"
#include "../program/landscape.h"
#include "treegraphicsview.h"
#include "../tree/square/squaretreegraphicsscene.h"
#include <QCloseEvent>
#include "../history/history.h"
#include "singleorganismwidget.h"
#include "landscapehistorywidget.h"
#include "../tree/curved/curvedtreegraphicsscene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    SingleOrganismWidget * m_historyOrganismWidget;
    LandscapeHistoryWidget * m_landscapeHistoryWidget;
    EnvironmentGraphicsScene m_graphicsScene;
    CurvedTreeGraphicsScene * m_curvedTreeGraphicsScene;
    SquareTreeGraphicsScene * m_squareTreeGraphicsScene;

    bool simulationIsRunning() const {return m_timer.isActive();}
    void startSimulation() {startStopSimulation(true);}
    void stopSimulation() {startStopSimulation(false);}
    void setHistorySpecies(int species);
    void setHistoryDate(long long date);
    void clearHistorySpecies();
    void clearHistoryDate();
    void showHistoryTip();
    void hideHistoryTip();
    void updateCurrentTreeStyle();


protected:
     void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;
    GraphicsViewZoom * m_graphicsViewZoom;
    QTimer m_timer;
    bool m_resumeSimulationAfterSave;
    QString m_autosavePath;
    bool m_justSaved;

    void updateTimeDisplay();
    bool checkForExtinction();
    void showSimulation();
    void hideSimulation();
    void saveSimulationAutomatic();
    void saveSimulation(QString fullFileName, QString message);
    void loadSimulation(QString fullFileName);

public slots:
    void finishedSaving();
    void finishedLoading();

private slots:
    void simulationSpeedChanged();
    void zoomSpinBoxChanged();
    void zoomedWithMouseWheel();
    void openLandscapeEditDialog();
    void openSettingsDialog();
    void startStopSimulationButtonPressed();
    void startStopSimulation(bool start);
    void advanceOneTick();
    void resetSimulation();
    void saveTreeToFile();
    void treeVerticalScaleChanged();
    void showHideHistory(bool show);
    void treeStyleChanged();
    void saveSimulationManual();
    void loadSimulationPrompt();
};

#endif // MAINWINDOW_H
