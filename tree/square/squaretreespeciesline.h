#ifndef SQUARETREESPECIESLINE_H
#define SQUARETREESPECIESLINE_H

#include <QGraphicsLineItem>
#include "../../program/globals.h"
#include <QPen>

#ifndef Q_MOC_RUN
#include "boost/shared_ptr.hpp"
#endif // Q_MOC_RUN

class Species;
class SquareTreeHorizontalLine;

class SquareTreeSpeciesLine : public QGraphicsLineItem
{
public:
    SquareTreeSpeciesLine(boost::shared_ptr<Species> species, double xFraction, int startStep,
                          double height, double width, QPen pen);
    ~SquareTreeSpeciesLine();

    boost::shared_ptr<Species> m_species;
    double m_xFraction;
    int m_startStep;
    int m_endStep;
    HighlightState m_highlightState;
    double m_lineStartY;
    double m_lineEndY;
    double m_highlightedY;
    double m_lineX;
    QPen m_pen;
    SquareTreeHorizontalLine * m_treeHorizontalLine;
    SpeciesState m_state;

    void updateSpeciesLine(double height, double width, QPen pen);
    void highlight(HighlightState highlightState, double y = 0.0);
    void unhighlight();
    bool containsPoint(double x, double y, double margin, double * xDistance);
    bool isLeafNode() const {return m_state == CURRENT || m_state == EXTINCT;}
    bool isNotLeafNode() const {return !isLeafNode();}
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
};


#endif // SQUARETREESPECIESLINE_H
