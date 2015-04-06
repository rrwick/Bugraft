#ifndef LANDSCAPEHISTORYWIDGET_H
#define LANDSCAPEHISTORYWIDGET_H

#include <QWidget>
#include <QPixmap>

class LandscapeHistoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LandscapeHistoryWidget(QWidget *parent = 0);

    QPixmap * m_landscapeThumbnail;
    std::vector< std::pair<float, float> > * m_organismPositions;


protected:
    void paintEvent(QPaintEvent * event);

};

#endif // LANDSCAPEHISTORYWIDGET_H
