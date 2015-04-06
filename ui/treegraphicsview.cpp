#include "treegraphicsview.h"

#include "../tree/square/squaretreegraphicsscene.h"
#include "../tree/curved/curvedtreegraphicsscene.h"
#include <QScrollBar>
#include "../ui/mainwindow.h"
#include "../program/globals.h"
#include "../program/settings.h"

TreeGraphicsView::TreeGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    setRenderHint(QPainter::Antialiasing, true);
}




void TreeGraphicsView::resizeEvent(QResizeEvent * event)
{
    sendWidthToScene();
    g_mainWindow->updateCurrentTreeStyle();
    QGraphicsView::resizeEvent(event);
}


void TreeGraphicsView::sendWidthToScene()
{
    int widthToSend =  width() - verticalScrollBar()->width() - 5;

    if (m_squareTreeGraphicsScene != 0)
        m_squareTreeGraphicsScene->m_width = widthToSend;
    if (m_curvedTreeGraphicsScene != 0)
        m_curvedTreeGraphicsScene->m_width = widthToSend;
}



