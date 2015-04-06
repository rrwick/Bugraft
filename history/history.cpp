#include "history.h"

#include "../program/settings.h"
#include "../program/globals.h"
#include "../program/landscape.h"
#include "../ui/mainwindow.h"

History::History() :
    m_maxBoundingBox(0.0, 0.0, 0.0, 0.0)
{
}


History::~History()
{
}


void History::logHistory()
{
    m_historyPoints.push_back(boost::shared_ptr<HistoryDataPoint>(new HistoryDataPoint(g_landscape->m_elapsedTime,
                                                                                       g_landscape->getRandomGenomeForEachSpecies(),
                                                                                       g_landscape->getCurrentParameters(),
                                                                                       g_landscape->getOrganismLocationsForEachSpecies())));

    //Loop through the newly added organisms and enlarge the max bounding
    //box to accomodate them all.
    for (std::map<int, Organism *>::iterator i = m_historyPoints.back()->m_randomOrganismForEachSpecies.begin();
         i != m_historyPoints.back()->m_randomOrganismForEachSpecies.end(); ++i)
    {
        possiblyEnlargeMaxBoundingBox(i->second->boundingRect());
    }

    g_mainWindow->m_historyOrganismWidget->m_boundingRect = m_maxBoundingBox;
}


void History::setMaxBoundingBoxAfterLoad()
{
    //Loop through all history organisms and enlarge the max bounding
    //box to accomodate them all.
    for (size_t i = 0; i < m_historyPoints.size(); ++i)
    {
        boost::shared_ptr<HistoryDataPoint> point = m_historyPoints[i];
        for (std::map<int, Organism *>::iterator j = point->m_randomOrganismForEachSpecies.begin();
             j != point->m_randomOrganismForEachSpecies.end(); ++j)
        {
            possiblyEnlargeMaxBoundingBox(j->second->boundingRect());
        }
    }
}


void History::possiblyEnlargeMaxBoundingBox(QRectF newBox)
{
    double currentWidth = m_maxBoundingBox.width();
    double currentHeight = m_maxBoundingBox.height();
    double newWidth = currentWidth;
    double newHeight = currentHeight;

    if (newBox.width() > currentWidth)
        newWidth = newBox.width();
    if (newBox.height() > currentHeight)
        newHeight = newBox.height();

    if (newWidth > currentWidth || newHeight > currentHeight)
        m_maxBoundingBox = QRectF(newWidth / 2.0, newHeight / 2.0, newWidth, newHeight);
}


void History::reset()
{
    m_historyPoints.clear();
    m_maxBoundingBox = QRectF(0.0, 0.0, 0.0, 0.0);
}


long long History::getClosestLoggedDate(double date)
{
    return getClosestHistoryPoint(date)->m_date;
}

boost::shared_ptr<HistoryDataPoint> History::getClosestHistoryPoint(double date)
{
    double indexDouble = date / g_settings->historyLogInterval;
    int indexInt = int(indexDouble + 0.5);  //Round to nearest integer

    if (indexInt < 0)
        indexInt = 0;
    if (indexInt > int(m_historyPoints.size()) - 1)
        indexInt = int(m_historyPoints.size()) - 1;

    return m_historyPoints[indexInt];
}

void History::remakeAllHistoryThumbnails()
{
    for (size_t i = 0; i < m_historyPoints.size(); ++i)
    {
        m_historyPoints[i]->makeThumbnail();
        ++g_landscapeThumbnailsMade;
    }
}
