#include "settings.h"
#include <QDir>

Settings::Settings()
{
    environmentWidth = 1024;
    environmentHeight = 576;
    simulationShown = true;

    treeStepHeight = 0.5;
    treeLineThickness = 1.0;
    treeWidgetMinimumWidth = 200;
    organismHighlightThickness = 20.0;
    treeSelectionDotDiameter = 6.0;
    organismHistoryDotDiameter = 1.5;
    treeLineSelectionMargin = 50.0;

    baseWaterLevel = 75.0;
    maxHabitableAltitudeAboveWaterLevel = 40.0;
    waterLevelVariationMagnitude = 30;
    waterLevelVariationRate = 50;
    landscapeShapeVariationRate = 50;
    waterLevelVariationRateFactor = 0.000002;
    landscapeChangeVariationRateFactor = 0.000002;

    landscapeChangeInterval = 25000;
    speciesCheckInterval = 25000;
    historyLogInterval = 25000;

    perlinZValue = 0.0;
    perlinOctaves = 5;
    perlinFrequency = 2.0;
    perlinPersistence = 0.4;

    minZoom = 0.05;
    maxZoom = 10.0;
    zoomFactor = 1.0005;
    organismScale = 0.02;
    walkingLegAngle = 0.15;

    singleOrganismWidgetBorder = 5.0;

    startingOrganismCount = 20;
    startingOrganismCircleRadius = 10;
    startingOrganismPositionAttempts = 1000;
    meanOrganismLifespan = 600.0;
    organismStepDistance = 0.3;
    organismDriftStepDistance = 0.8;
    organismAngleChange = 0.6;
    organismDriftingRotationRate = 0.1;
    organismDriftingAngleChange = 0.1;
    organismFullyGrownAge = 50;
    bumpFreeTimeBeforeGivingBirth = 180.0;

    chanceOfMutation = 0.1;
    maxMutationSize = 1.8;
    maximumDistanceForMating = 4.4;
    maximumDistanceForSameSpecies = 6.7;

    chanceOfDriftingOverWater = 0.00001;
    chanceOfDriftingOverAltitude = 0.00001;

    lineAngleReductionFactor = 0.003;
    maxLineAngle = 35.0;
    lineAngleCurveFactor = 0.05;
    lineAngleStraightenFactor = 0.003;
    maxSpacing = 0.2;

    rememberedPath = QDir::homePath();

    landscapeColors.push_back(QColor(0, 0, 140));
    landscapeColors.push_back(QColor(0, 0, 155));
    landscapeColors.push_back(QColor(0, 0, 255));
    landscapeColors.push_back(QColor(0, 128, 255));
    landscapeColors.push_back(QColor(240, 240, 64));
    landscapeColors.push_back(QColor(32, 160, 0));
    landscapeColors.push_back(QColor(38, 112, 19));
    landscapeColors.push_back(QColor(128, 128, 128));
    landscapeColors.push_back(QColor(255, 255, 255));

    landscapeColorAltitudes.push_back(-22.5);
    landscapeColorAltitudes.push_back(-15.0);
    landscapeColorAltitudes.push_back(-7.5);
    landscapeColorAltitudes.push_back(-1.0);
    landscapeColorAltitudes.push_back(0.5);
    landscapeColorAltitudes.push_back(12.5);
    landscapeColorAltitudes.push_back(25.0);
    landscapeColorAltitudes.push_back(40.0);
    landscapeColorAltitudes.push_back(50.0);

    landscapeColorNoise.push_back(0.0);
    landscapeColorNoise.push_back(0.0);
    landscapeColorNoise.push_back(0.0);
    landscapeColorNoise.push_back(0.0);
    landscapeColorNoise.push_back(0.5);
    landscapeColorNoise.push_back(0.1);
    landscapeColorNoise.push_back(0.1);
    landscapeColorNoise.push_back(0.3);
    landscapeColorNoise.push_back(0.0);


}


void Settings::rememberPath(QString path)
{
    rememberedPath = QFileInfo(path).absoluteDir().absolutePath();
}
