#include "squaretreehorizontalline.h"

#include "squaretreespeciesline.h"
#include "../../program/globals.h"
#include "../../program/settings.h"


SquareTreeHorizontalLine::SquareTreeHorizontalLine(std::vector<SquareTreeSpeciesLine *> branches,
                                       double height, double width, QPen pen) :
    m_branches(branches), m_highlighted(false)
{
    setZValue(-1.0); //Horizontal lines are drawn below species lines
    updateHorizontalLine(height, width, pen);
}

SquareTreeHorizontalLine::~SquareTreeHorizontalLine()
{

}


void SquareTreeHorizontalLine::updateHorizontalLine(double height, double width, QPen pen)
{

    double lineStartX = m_branches.front()->m_xFraction * width;
    double lineEndX = m_branches.back()->m_xFraction * width;
    double lineY = height - m_branches.front()->m_startStep * g_settings->treeStepHeight;

    setLine(lineStartX, lineY, lineEndX, lineY);

    if (m_highlighted)
        pen.setColor(Qt::red);
    setPen(pen);
}

void SquareTreeHorizontalLine::highlight()
{
    m_highlighted = true;
    for (size_t i = 0; i < m_branches.size(); ++i)
        m_branches[i]->highlight(WHOLE_LINE);
}

void SquareTreeHorizontalLine::unhighlight()
{
    m_highlighted = false;
    for (size_t i = 0; i < m_branches.size(); ++i)
        m_branches[i]->unhighlight();
}
