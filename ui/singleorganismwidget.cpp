#include "singleorganismwidget.h"

#include <QPainter>
#include "../bug/organism.h"
#include "../program/globals.h"
#include "../program/settings.h"
#include <QTransform>
#include <QRectF>


//This constructor is used when the widget is in the history section.
//In this case, it will show many possible organisms, and so its
//bounding rectangle isn't for any one of them, but rather a rectangle
//that can encompass all of the history organisms.
//The bounding rectangle will be set by the History object.
SingleOrganismWidget::SingleOrganismWidget(QWidget *parent) :
    QWidget(parent), m_boundingRect(0.0, 0.0, 0.0, 0.0), m_organism(0)
{
}



//This constructor is used when the widget is in the Organism Info Dialog.
//In this case, it will only ever show one organism, and so the bounding
//rectangle is set for that organism.
SingleOrganismWidget::SingleOrganismWidget(Organism * organism, QWidget *parent) :
    QWidget(parent), m_organism(organism)
{
    //Prevent the widget from getting too narrow.  This prevents the organism from
    //being drawn excessively small or even inverted (if it has a negative space in
    //which to be drawn).
    setMinimumSize(g_settings->singleOrganismWidgetBorder * 20,
                   g_settings->singleOrganismWidgetBorder * 20);

    m_boundingRect = m_organism->boundingRect();
}



void SingleOrganismWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Qt::white);

    if (m_organism == 0)
        return;

    painter.setRenderHint(QPainter::Antialiasing, true);

    //Set up the necessary transformation such that the organism is displayed in its entirity
    //in this widget.
    QTransform transform;
    double availableWidth = width() - 2 * g_settings->singleOrganismWidgetBorder;
    double availableHeight = height() - g_settings->singleOrganismWidgetBorder;

    QRectF rect = m_organism->boundingRect();
    double organismWidth = rect.width();
    double organismHeight = rect.height();

    double horizontalScaleToFit = availableWidth / organismWidth;
    double verticalScaleToFit = availableHeight / organismHeight;

    double finalScale;
    if (horizontalScaleToFit < verticalScaleToFit)
        //In this case, the drawing is limited by the available width.
        //Height at the top of the widget will go unused.
        finalScale = horizontalScaleToFit;
    else
        //In this case, the drawing is limited by the available height.
        //Width on both sides of the widget will go usused.
        finalScale = verticalScaleToFit;

    double requiredWidth = organismWidth * finalScale;
    double excessWidth = width() - requiredWidth;
    double shiftForHorizontalCentering = excessWidth / 2.0;

    double rightShift = -1.0 * rect.left() * finalScale + shiftForHorizontalCentering;
    double downShift = height() / 2.0;

    transform.translate(rightShift, downShift);
    transform.scale(finalScale, finalScale);

    painter.setTransform(transform);

    m_organism->paint(&painter, 0, 0);
}
