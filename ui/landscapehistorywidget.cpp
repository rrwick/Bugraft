#include "landscapehistorywidget.h"

#include <QPainter>
#include "../program/settings.h"
#include "../program/globals.h"

LandscapeHistoryWidget::LandscapeHistoryWidget(QWidget *parent) :
    QWidget(parent), m_landscapeThumbnail(0), m_organismPositions(0)
{
}



void LandscapeHistoryWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    if (m_landscapeThumbnail == 0)
    {
        painter.fillRect(0, 0, width(), height(), Qt::white);
        return;
    }

    painter.drawPixmap(0, 0, *m_landscapeThumbnail);

    if (m_organismPositions == 0)
        return;

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);

    double halfDotSize = g_settings->organismHistoryDotDiameter / 2.0;
    double scaleFactor = double(m_landscapeThumbnail->height()) / g_settings->environmentHeight;

    for (size_t i = 0; i < m_organismPositions->size(); ++i)
    {
        double x = (*m_organismPositions)[i].first * scaleFactor;
        double y = (*m_organismPositions)[i].second * scaleFactor;

        painter.drawEllipse(QPointF(x, y), halfDotSize, halfDotSize);
    }
}
