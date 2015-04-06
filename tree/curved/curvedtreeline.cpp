#include "curvedtreeline.h"
#include "../../bug/species.h"
#include "../../program/settings.h"
#include "math.h"
#include "../../program/landscape.h"


CurvedTreeLine::CurvedTreeLine(boost::shared_ptr<Species> species, long long startDate) :
    m_x(0.5), m_y(0.0),
    m_direction(0.0),
    m_preferredX(0.5),
    m_current(true),
    m_highlightState(NO_HIGHLIGHT),
    m_startDate(startDate),
    m_endDate(g_landscape->m_elapsedTime),
    m_species(species),
    m_sceneWidth(0.0),
    m_sceneHeight(0.0),
    m_wideningFactor(1.0)
{
    m_points.push_back(QPointF(m_x, m_y));
    m_species->m_curvedTreeLine = this;
}

CurvedTreeLine::CurvedTreeLine(CurvedTreeLine * parent, boost::shared_ptr<Species> species, long long startDate) :
    m_x(parent->m_x), m_y(parent->m_y),
    m_direction(parent->m_direction),
    m_preferredX(parent->m_preferredX),
    m_current(true),
    m_highlightState(NO_HIGHLIGHT),
    m_startDate(startDate),
    m_endDate(g_landscape->m_elapsedTime),
    m_species(species),
    m_sceneWidth(0.0),
    m_sceneHeight(0.0),
    m_wideningFactor(1.0)
{
    m_points.push_back(QPointF(m_x, m_y));
    m_species->m_curvedTreeLine = this;
}

CurvedTreeLine::~CurvedTreeLine()
{
}


SpeciesState CurvedTreeLine::growOneStep(long long date)
{
    if (m_species->getStateAtDate(date) == EXTINCT)
    {
        m_current = false;
        return EXTINCT;
    }

    if (m_species->getStateAtDate(date) == SPLIT)
    {
        for (size_t i = 0; i < m_species->m_childSpecies.size(); ++i)
            m_children.push_back(new CurvedTreeLine(this, m_species->m_childSpecies[i], date));

        m_current = false;
        return SPLIT;
    }

    m_endDate = date;

    //The line's angle is deviated away from the
    //vertical proportionally to how far away it is
    //from its preferred X.
    m_direction += (m_preferredX - m_x) * g_settings->lineAngleCurveFactor;

    //The line's angle is returned back toward the
    //vertical proportionally to how deviated from
    //the vertical it currently is.
    m_direction -= m_direction * g_settings->lineAngleStraightenFactor;

    //Limit the direction
    if (m_direction > g_settings->maxLineAngle)
        m_direction = g_settings->maxLineAngle;
    if (m_direction < -g_settings->maxLineAngle)
        m_direction = -g_settings->maxLineAngle;

    //The line always moves up by the same amount,
    //but the distance it moves over depends on
    //its angle.
    m_y += 1.0;
    m_x += g_settings->lineAngleReductionFactor * tan(m_direction / 57.2957795130823);

    m_points.push_back(QPointF(m_x, m_y));

    return CURRENT;
}


void CurvedTreeLine::updateLine(double height, double width, QPen pen, double wideningFactor)
{
    m_sceneWidth = width;
    m_sceneHeight = height;
    m_wideningFactor = wideningFactor;

    setPath(getPainterPath());
    setPen(pen);

    m_pen = pen;
    if (m_highlightState == WHOLE_LINE)
        m_pen.setColor(Qt::red);
}


QPainterPath CurvedTreeLine::getPainterPath()
{
    QPainterPath path(getPathPoint(0));

    for (size_t i = 1; i < m_points.size(); ++i)
        path.lineTo(getPathPoint(int(i)));

    return path;
}


QPainterPath CurvedTreeLine::getPainterPathUpToHighlightedPoint()
{
    QPainterPath path(getPathPoint(0));

    for (size_t i = 1; i < m_points.size(); ++i)
    {
        QPointF nextPoint = getPathPoint(int(i));
        if (nextPoint.y() > m_highlightedY || almostEqual(nextPoint.y(), m_highlightedY))
            path.lineTo(nextPoint);
        else
            break;
    }

    return path;
}

QPainterPath CurvedTreeLine::getPainterPathAfterHighlightedPoint()
{
    QPainterPath path;

    bool firstPointFound = false;
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        QPointF nextPoint = getPathPoint(int(i));
        if (nextPoint.y() > m_highlightedY && !almostEqual(nextPoint.y(), m_highlightedY))
            continue;

        if (firstPointFound)
            path.lineTo(nextPoint);
        else
        {
            path.moveTo(nextPoint);
            firstPointFound = true;
        }
    }
    return path;
}



//This function uses the given width and height of the tree's scene
//to translate one of the line's stored coordinates (which are in
//relative terms) to absolute coordinates.
QPointF CurvedTreeLine::getPathPoint(int index)
{
    double x = m_points[index].x();
    double distanceFromMiddle = 0.5 - x;
    double newDistanceFromMiddle = m_wideningFactor * distanceFromMiddle;
    double newX = 0.5 - newDistanceFromMiddle;

    return QPointF(m_sceneWidth * newX,
                   m_sceneHeight - (g_settings->treeStepHeight * m_points[index].y()));
}



void CurvedTreeLine::highlight(HighlightState highlightState, double y)
{
    m_highlightState = highlightState;
    m_highlightedY = y;

    for (size_t i = 0; i < m_children.size(); ++i)
        m_children[i]->highlight(WHOLE_LINE);
}

void CurvedTreeLine::unhighlight()
{
    m_highlightState = NO_HIGHLIGHT;

    for (size_t i = 0; i < m_children.size(); ++i)
        m_children[i]->unhighlight();
}

//For a given y value, this function returns the x value on the line.
//It specifically gets an x value that corresponds to a point - i.e.
//it does not interpolate between points.
double CurvedTreeLine::getX(int y)
{
    //Find the point with the y values that most closely
    //matches the parameter.
    size_t closestYIndex = 0;
    double closestYDistance = fabs(m_points[0].y() - y);
    for (size_t i = 1; i < m_points.size(); ++i)
    {
        double yDistance = fabs(m_points[i].y() - y);
        if (yDistance < closestYDistance)
        {
            closestYDistance = yDistance;
            closestYIndex = i;
        }
    }

    return getPathPoint(int(closestYIndex)).x();
}


bool CurvedTreeLine::containsPoint(double x, double y, double margin, double * distanceToLine)
{
    QPointF p(x, y);

    double closestDistanceSquaredToPoint = std::numeric_limits<double>::max();
    for (size_t i = 1; i < m_points.size(); ++i)
    {
        double a = distanceSquared(p, getPathPoint(int(i)));
        if (a < closestDistanceSquaredToPoint)
            closestDistanceSquaredToPoint = a;
    }

    *distanceToLine = sqrt(closestDistanceSquaredToPoint);

    return *distanceToLine < margin;
}


double CurvedTreeLine::distanceSquared(QPointF a, QPointF b)
{
    double xDiff = a.x() - b.x();
    double yDiff = a.y() - b.y();
    return xDiff * xDiff + yDiff * yDiff;
}

double CurvedTreeLine::distance(QPointF a, QPointF b)
{
    return sqrt(distanceSquared(a,b));
}


void CurvedTreeLine::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (m_highlightState == PARTIAL_LINE)
    {
        painter->setPen(m_pen);
        painter->drawPath(getPainterPathUpToHighlightedPoint());

        QPen redPen(m_pen);
        redPen.setColor(Qt::red);
        painter->setPen(redPen);
        painter->drawPath(getPainterPathAfterHighlightedPoint());
    }
    else
    {
        painter->setPen(m_pen);
        painter->drawPath(path());
    }
}


bool CurvedTreeLine::newlyCreated()
{
    return m_points.size() == 1;
}


bool CurvedTreeLine::almostEqual(double a, double b)
{
    return fabs(a - b) < 0.001;
}
