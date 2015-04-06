#ifndef CURVEDTREELINE_H
#define CURVEDTREELINE_H

#include <vector>
#include <QPointF>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include "../../program/globals.h"
#include <QPen>
#include <QPainter>

#ifndef Q_MOC_RUN
#include "boost/shared_ptr.hpp"
#endif // Q_MOC_RUN

class Species;

class CurvedTreeLine : public QGraphicsPathItem
{
public:
    CurvedTreeLine(boost::shared_ptr<Species> species, long long startDate);
    CurvedTreeLine(CurvedTreeLine * parent, boost::shared_ptr<Species> species, long long startDate);
    ~CurvedTreeLine();

    double m_x, m_y;
    double m_direction;
    double m_preferredX;
    bool m_current;
    HighlightState m_highlightState;
    double m_highlightedY;
    QPen m_pen;
    long long m_startDate;
    long long m_endDate;
    boost::shared_ptr<Species> m_species;
    double m_sceneWidth;
    double m_sceneHeight;
    std::vector<QPointF> m_points;
    std::vector<CurvedTreeLine *> m_children;

    void updateLine(double height, double width, QPen pen, double wideningFactor);
    SpeciesState growOneStep(long long date);
    QPainterPath getPainterPath();
    QPainterPath getPainterPathUpToHighlightedPoint();
    QPainterPath getPainterPathAfterHighlightedPoint();
    QPointF getPathPoint(int index);
    void highlight(HighlightState highlightState, double y = 0);
    void unhighlight();
    bool containsPoint(double x, double y, double margin, double * distanceToLine);
    double getX(int y);
    bool newlyCreated();
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

private:
    double m_wideningFactor;

    double distanceSquared(QPointF a, QPointF b);
    double distance(QPointF a, QPointF b);
    bool almostEqual(double a, double b);

};

#endif // CURVEDTREELINE_H
