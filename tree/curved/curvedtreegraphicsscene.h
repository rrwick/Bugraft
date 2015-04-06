#ifndef CURVEDTREEGRAPHICSSCENE_H
#define CURVEDTREEGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include "../../bug/species.h"
#include <list>
#include "curvedtreeline.h"
#include <QPen>
#include <QGraphicsEllipseItem>

#ifndef Q_MOC_RUN
#include "boost/shared_ptr.hpp"
#endif // Q_MOC_RUN

class CurvedTreeGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CurvedTreeGraphicsScene(QGraphicsView * landscapeView);
    ~CurvedTreeGraphicsScene();

    double m_height;
    double m_width;
    QPen m_treePen;
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
    std::list<CurvedTreeLine *> m_lines;
    boost::shared_ptr<QGraphicsEllipseItem> m_selectionDot;
    boost::shared_ptr<Species> m_startingSpecies;
    long long m_selectionDate;
    int m_stepCount;
    CurvedTreeLine * m_highlightedLine;

    void cleanUp();
    QPen getTreePen();
    void setTreeLinePreferredX();
    int getCurrentLineCount();
    void mousePressOrMove(QGraphicsSceneMouseEvent * event);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

};

#endif // CURVEDTREEGRAPHICSSCENE_H
