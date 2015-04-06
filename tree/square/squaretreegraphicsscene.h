#ifndef SQUARETREEGRAPHICSSCENE_H
#define SQUARETREEGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include "../../bug/species.h"
#include <vector>
#include "squaretreespeciesline.h"
#include "squaretreehorizontalline.h"
#include "QPen"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>

#ifndef Q_MOC_RUN
#include "boost/shared_ptr.hpp"
#endif // Q_MOC_RUN

class SquareTreeGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    SquareTreeGraphicsScene(QGraphicsView * landscapeView);
    ~SquareTreeGraphicsScene();

    double m_height;
    double m_width;
    QPen m_treePen;
    double m_spacing;
    QGraphicsView * m_landscapeView;
    long long m_lastLoggedDateInTree;

    void start(boost::shared_ptr<Species> startingSpecies);
    void growTreeOneStep(long long date);
    void updateTree();
    void highlight(double x, double y);
    void stopHighlighting();
    void clearLandscapeThumbnail();
    void rebuildTree();

private:
    std::list<SquareTreeSpeciesLine *> m_speciesLines;
    std::list<SquareTreeHorizontalLine *> m_horizontalLines;
    boost::shared_ptr<QGraphicsEllipseItem> m_selectionDot;
    boost::shared_ptr<Species> m_startingSpecies;
    long long m_selectionDate;
    int m_stepCount;
    SquareTreeSpeciesLine * m_highlightedLine;

    void cleanUp();
    QPen getTreePen();
    void updateXPositions();
    void mousePressOrMove(QGraphicsSceneMouseEvent * event);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
};

#endif // SQUARETREEGRAPHICSSCENE_H
