#include "waterlevelvisualaid.h"

#include <QPainter>
#include "../program/landscape.h"
#include "../program/globals.h"
#include "../program/settings.h"
#include <QPainterPath>

WaterLevelVisualAid::WaterLevelVisualAid(double baseWaterLevel, int waterLevelVariationMagnitude, int waterLevelVariationRate,
                                         QWidget *parent) :
    QWidget(parent),
    m_baseWaterLevel(baseWaterLevel), m_waterLevelVariationRate(waterLevelVariationRate),
    m_waterLevelVariationMagnitude(waterLevelVariationMagnitude), m_graphLineThickness(4.0)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}


void WaterLevelVisualAid::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), QBrush(Qt::white));

    QPen gridLinePen(QBrush(Qt::gray), 0.5);
    painter.setPen(gridLinePen);

    //Horizontal grid lines
    painter.drawLine(0, 0.0, width(), 0.0);
    painter.drawLine(0, height() * 0.25, width(), height() * 0.25);
    painter.drawLine(0, height() * 0.5, width(), height() * 0.5);
    painter.drawLine(0, height() * 0.75, width(), height() * 0.75);

    //Vertical grid lines
    painter.drawLine(width() * 0.125, 0, width() * 0.125, height());
    painter.drawLine(width() * 0.25, 0, width() * 0.25, height());
    painter.drawLine(width() * 0.375, 0, width() * 0.375, height());
    painter.drawLine(width() * 0.5, 0, width() * 0.5, height());
    painter.drawLine(width() * 0.625, 0, width() * 0.625, height());
    painter.drawLine(width() * 0.75, 0, width() * 0.75, height());
    painter.drawLine(width() * 0.875, 0, width() * 0.875, height());


    painter.setRenderHint(QPainter::Antialiasing, true);

    double z = g_settings->waterLevelPerlinZValue;

    QPainterPath graphLine;
    graphLine.moveTo(0, getWaterLevelLineY(z));

    for (int i = 0; i < width(); ++i)
    {
        graphLine.lineTo(i, getWaterLevelLineY(z));
        z += m_waterLevelVariationRate * g_settings->waterLevelVariationRateFactor * 200000.0 / width();
    }
    QPen waterLinePen(QBrush(Qt::blue), m_graphLineThickness);
    painter.setPen(waterLinePen);
    painter.drawPath(graphLine);
}

double WaterLevelVisualAid::getWaterLevelLineY(double z)
{
    double waterLevel = g_landscape->getWaterLevelFromPerlinFunction(m_baseWaterLevel, m_waterLevelVariationMagnitude, z);

    double fractionFull = waterLevel / 100.0;
    double distanceFromBottom = height() * fractionFull;
    double distanceFromTop = height() - distanceFromBottom;

    double halfGraphLineThickness = m_graphLineThickness / 2.0;
    if (distanceFromTop < halfGraphLineThickness)
        distanceFromTop = halfGraphLineThickness;
    if (distanceFromTop > height() - halfGraphLineThickness)
        distanceFromTop = height() - halfGraphLineThickness;

    return distanceFromTop;
}
