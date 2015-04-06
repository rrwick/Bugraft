#ifndef TREEGRAPHICSVIEW_H
#define TREEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QSize>

class SquareTreeGraphicsScene;
class CurvedTreeGraphicsScene;

class TreeGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TreeGraphicsView(QWidget *parent = 0);

    SquareTreeGraphicsScene * m_squareTreeGraphicsScene;
    CurvedTreeGraphicsScene * m_curvedTreeGraphicsScene;
    void sendWidthToScene();

protected:
    void resizeEvent(QResizeEvent * event);


};

#endif // TREEGRAPHICSVIEW_H
