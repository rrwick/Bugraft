#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <QColor>

#ifndef Q_MOC_RUN
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#endif // Q_MOC_RUN
namespace boost {namespace serialization {class access;}}


class Settings
{
public:
    Settings();

    int environmentWidth;
    int environmentHeight;
    bool simulationShown;

    double treeStepHeight;
    double treeLineThickness;
    int treeWidgetMinimumWidth;
    double organismHighlightThickness;
    double treeSelectionDotDiameter;
    double organismHistoryDotDiameter;
    double treeLineSelectionMargin;

    double baseWaterLevel;
    double maxHabitableAltitudeAboveWaterLevel;
    int waterLevelVariationMagnitude;
    int waterLevelVariationRate;
    int landscapeShapeVariationRate;
    double waterLevelVariationRateFactor;
    double landscapeChangeVariationRateFactor;

    int landscapeChangeInterval;
    int speciesCheckInterval;
    int historyLogInterval;

    double perlinZValue;
    int perlinOctaves;
    double perlinFrequency;
    double perlinPersistence;

    double waterLevelPerlinXValue;
    double waterLevelPerlinYValue;
    double waterLevelPerlinZValue;

    double minZoom;
    double maxZoom;
    double zoomFactor;
    double organismScale;
    double walkingLegAngle;

    double singleOrganismWidgetBorder;

    int startingOrganismCount;
    int startingOrganismCircleRadius;
    int startingOrganismPositionAttempts;
    double meanOrganismLifespan;
    double organismStepDistance;
    double organismDriftStepDistance;
    double organismAngleChange;
    double organismDriftingRotationRate;
    double organismDriftingAngleChange;
    int organismFullyGrownAge;
    int bumpFreeTimeBeforeGivingBirth;

    double chanceOfMutation;
    double maxMutationSize;
    double maximumDistanceForMating;
    double maximumDistanceForSameSpecies;

    double chanceOfDriftingOverWater;
    double chanceOfDriftingOverAltitude;

    //Curved tree settings
    double lineAngleReductionFactor;
    double maxLineAngle;
    double lineAngleCurveFactor;
    double lineAngleStraightenFactor;
    double maxSpacing;

    QString rememberedPath;

    std::vector<QColor> landscapeColors;
    std::vector<double> landscapeColorAltitudes;
    std::vector<double> landscapeColorNoise;

    void rememberPath(QString path);


    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive & ar, const unsigned)
    {
        ar & startingOrganismCount;
        ar & meanOrganismLifespan;
        ar & bumpFreeTimeBeforeGivingBirth;

        ar & chanceOfMutation;
        ar & maxMutationSize;
        ar & maximumDistanceForMating;
        ar & maximumDistanceForSameSpecies;

        ar & chanceOfDriftingOverWater;
        ar & chanceOfDriftingOverAltitude;
        ar & maxHabitableAltitudeAboveWaterLevel;

        ar & baseWaterLevel;
        ar & waterLevelVariationMagnitude;
        ar & waterLevelVariationRate;
        ar & landscapeShapeVariationRate;

        ar & perlinZValue;
        ar & perlinFrequency;
        ar & perlinPersistence;

        ar & waterLevelPerlinXValue;
        ar & waterLevelPerlinYValue;
        ar & waterLevelPerlinZValue;
    }
};

#endif // SETTINGS_H
