#ifndef SQUARETREEHORIZONTALLINE_H
#define SQUARETREEHORIZONTALLINE_H

#include <QGraphicsLineItem>
#include <QPen>
#include <vector>


namespace boost {namespace serialization {class access;}}

class SquareTreeSpeciesLine;

class SquareTreeHorizontalLine : public QGraphicsLineItem
{
public:
    SquareTreeHorizontalLine(std::vector<SquareTreeSpeciesLine *> branches, double height, double width, QPen pen);
    ~SquareTreeHorizontalLine();

    std::vector<SquareTreeSpeciesLine *> m_branches;
    bool m_highlighted;

    void highlight();
    void unhighlight();
    void updateHorizontalLine(double height, double width, QPen pen);
};

#endif // SQUARETREEHORIZONTALLINE_H
