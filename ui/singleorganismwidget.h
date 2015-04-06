#ifndef SINGLEORGANISMWIDGET_H
#define SINGLEORGANISMWIDGET_H

#include <QWidget>
#include <QRectF>

class Organism;

class SingleOrganismWidget : public QWidget
{
    Q_OBJECT
public:
    SingleOrganismWidget(QWidget *parent = 0);
    SingleOrganismWidget(Organism * organism, QWidget *parent = 0);

    QRectF m_boundingRect;
    Organism * m_organism;

protected:
    void paintEvent(QPaintEvent * event);

};

#endif // SINGLEORGANISMWIDGET_H
