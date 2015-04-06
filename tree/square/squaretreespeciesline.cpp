#include "squaretreespeciesline.h"

#include "../../program/settings.h"
#include "../../bug/species.h"
#include <math.h>
#include "squaretreehorizontalline.h"
#include <QPainter>


SquareTreeSpeciesLine::SquareTreeSpeciesLine(boost::shared_ptr<Species> species, double xFraction, int startStep,
                                 double height, double width, QPen pen) :
    m_species(species),
    m_xFraction(xFraction),
    m_startStep(startStep),
    m_endStep(startStep),
    m_highlightState(NO_HIGHLIGHT),
    m_pen(pen),
    m_treeHorizontalLine(0),
    m_state(CURRENT)
{
    updateSpeciesLine(height, width, pen);
    m_species->m_squareTreeSpeciesLine = this;
}

SquareTreeSpeciesLine::~SquareTreeSpeciesLine()
{

}

void SquareTreeSpeciesLine::updateSpeciesLine(double height, double width, QPen pen)
{
    m_lineStartY = height - m_startStep * g_settings->treeStepHeight;
    m_lineEndY = height - m_endStep * g_settings->treeStepHeight;
    m_lineX = m_xFraction * width;

    setLine(m_lineX, m_lineStartY, m_lineX, m_lineEndY);

    setPen(pen);
    m_pen = pen;
    if (m_highlightState == WHOLE_LINE)
        m_pen.setColor(Qt::red);
}

bool SquareTreeSpeciesLine::containsPoint(double x, double y, double margin, double * xDistance)
{
    if (y > m_lineStartY || y < m_lineEndY)
        return false;

    *xDistance = fabs(m_lineX - x);

    return *xDistance < margin;
}


void SquareTreeSpeciesLine::highlight(HighlightState highlightState, double y)
{
    m_highlightState = highlightState;
    m_highlightedY = y;
    if (m_treeHorizontalLine != 0)
        m_treeHorizontalLine->highlight();
}

void SquareTreeSpeciesLine::unhighlight()
{
    m_highlightState = NO_HIGHLIGHT;
    if (m_treeHorizontalLine != 0)
        m_treeHorizontalLine->unhighlight();
}

void SquareTreeSpeciesLine::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (m_highlightState == PARTIAL_LINE)
    {
        painter->setPen(m_pen);
        painter->drawLine(m_lineX, m_lineStartY, m_lineX, m_highlightedY);

        QPen redPen(m_pen);
        redPen.setColor(Qt::red);
        painter->setPen(redPen);
        painter->drawLine(m_lineX, m_highlightedY, m_lineX, m_lineEndY);
    }
    else
    {
        painter->setPen(m_pen);
        painter->drawLine(m_lineX, m_lineStartY, m_lineX, m_lineEndY);
    }
}
