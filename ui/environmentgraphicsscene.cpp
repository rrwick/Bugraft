#include "environmentgraphicsscene.h"

#include <QBrush>


EnvironmentGraphicsScene::EnvironmentGraphicsScene(QObject * parent) :
    QGraphicsScene(parent)
{
}



void EnvironmentGraphicsScene::drawBackground(QPainter * painter, const QRectF & rect)
{
    painter->drawPixmap(rect, m_background, rect);
}
