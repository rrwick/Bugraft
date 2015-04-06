#ifndef ENVIRONMENTGRAPHICSSCENE_H
#define ENVIRONMENTGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>
#include <QPixmap>

class EnvironmentGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit EnvironmentGraphicsScene(QObject *parent = 0);
    QPixmap m_background;

protected:
    void drawBackground(QPainter * painter, const QRectF & rect);

};

#endif // ENVIRONMENTGRAPHICSSCENE_H
