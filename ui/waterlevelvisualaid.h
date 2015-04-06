#ifndef WATERLEVELVISUALAID_H
#define WATERLEVELVISUALAID_H

#include <QWidget>

class Landscape;

class WaterLevelVisualAid : public QWidget
{
    Q_OBJECT
public:
    explicit WaterLevelVisualAid(double baseWaterLevel,
                                 int waterLevelVariationMagnitude, int waterLevelVariationRate,
                                 QWidget *parent = 0);

    double m_baseWaterLevel;
    int m_waterLevelVariationRate;
    int m_waterLevelVariationMagnitude;

protected:
    void paintEvent(QPaintEvent *);

private:
    double m_graphLineThickness;

    double getWaterLevelLineY(double z);
};

#endif // WATERLEVELVISUALAID_H
