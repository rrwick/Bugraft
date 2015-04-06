#include "curvedtreegraphicsscene.h"
#include "../../program/settings.h"
#include "../../program/globals.h"
#include "../../program/landscape.h"
#include "../../program/randomnumbers.h"
#include <QGraphicsPathItem>
#include "../../ui/mainwindow.h"

CurvedTreeGraphicsScene::CurvedTreeGraphicsScene(QGraphicsView * landscapeView) :
    m_height(0), m_width(g_settings->treeWidgetMinimumWidth),
    m_treePen(getTreePen()),
    m_landscapeView(landscapeView),
    m_lastLoggedDateInTree(0),
    m_selectionDot(),
    m_startingSpecies(0),
    m_selectionDate(-1),
    m_stepCount(0),
    m_highlightedLine(0)
{
}

CurvedTreeGraphicsScene::~CurvedTreeGraphicsScene()
{
    cleanUp();
}

void CurvedTreeGraphicsScene::cleanUp()
{
    clear();
    m_lines.clear();
}


void CurvedTreeGraphicsScene::start(boost::shared_ptr<Species> startingSpecies)
{
    cleanUp();

    m_stepCount = 0;
    m_startingSpecies = startingSpecies;
    m_lastLoggedDateInTree = 0;

    CurvedTreeLine * firstLine = new CurvedTreeLine(startingSpecies, 0);

    addItem(firstLine);
    m_lines.push_back(firstLine);
}

void CurvedTreeGraphicsScene::growTreeOneStep(long long date)
{
    ++m_stepCount;
    m_lastLoggedDateInTree = date;

    bool currentLinesChanged = false;
    for (std::list<CurvedTreeLine *>::iterator i = m_lines.begin(); i != m_lines.end(); ++i)
    {
        if ((*i)->m_current)
        {
            SpeciesState lineState = (*i)->growOneStep(date);
            if (lineState == SPLIT ||
                    lineState == EXTINCT)
                currentLinesChanged = true;

            if (lineState == SPLIT)
            {
                for (size_t j = 0; j < (*i)->m_children.size(); ++j)
                {
                    m_lines.insert(i, (*i)->m_children[j]);
                    addItem((*i)->m_children[j]);
                }
            }
        }
    }

    if (currentLinesChanged)
    {
        setTreeLinePreferredX();

        //Newly made lines need to grow another step to catch up with the
        //rest of the lines.
        for (std::list<CurvedTreeLine *>::iterator i = m_lines.begin(); i != m_lines.end(); ++i)
        {
            if ((*i)->newlyCreated())
                (*i)->growOneStep(date);
        }
    }

    //If something was highlighted in the scene (i.e. the mouse was down),
    //it is necessary to shift some positions downward.
    if (m_selectionDot != NULL)
        m_selectionDot->moveBy(0, g_settings->treeStepHeight);
    if (m_highlightedLine != 0)
        m_highlightedLine->m_highlightedY += g_settings->treeStepHeight;
}

void CurvedTreeGraphicsScene::updateTree()
{
    m_height = m_stepCount * g_settings->treeStepHeight;
    setSceneRect(0.0, 0.0, m_width, m_height);
    QPen pen = getTreePen();

    //Since curved trees can be very narrow during early stages
    //of a simulation, it will be drawn wider if it takes up less
    //than a defined fraction of the available width.

    double minX = 1.0;
    double maxX = 0.0;
    for (std::list<CurvedTreeLine *>::iterator i = m_lines.begin(); i != m_lines.end(); ++i)
    {
        CurvedTreeLine * line = *i;
        for (size_t j = 0; j < line->m_points.size(); ++j)
        {
            double pointX = line->m_points[j].x();
            if (pointX < minX)
                minX = pointX;
            if (pointX > maxX)
                maxX = pointX;
        }
    }
    double rangeFromCentre = std::max(maxX - 0.5, 0.5 - minX);

    //0.45 is the target range: taking up most of the available space.
    //If it is less than this, we'll widen the tree.
    //If it is more, the widening factor will be less than 1 and the
    //tree will be narrowed.
    double wideningFactor = 0.45 / rangeFromCentre;

    //A very narrow tree will look odd if overly widened, so limit
    //widening to a factor of 20.
    if (wideningFactor > 20.0)
        wideningFactor = 20.0;

    for (std::list<CurvedTreeLine *>::iterator i = m_lines.begin(); i != m_lines.end(); ++i)
        (*i)->updateLine(m_height, m_width, pen, wideningFactor);
}


QPen CurvedTreeGraphicsScene::getTreePen()
{
    double penWidth = g_settings->treeLineThickness;
    double widthOverMinimum = m_width - g_settings->treeWidgetMinimumWidth;
    penWidth += g_settings->treeLineThickness * widthOverMinimum / g_settings->treeWidgetMinimumWidth / 3.0;

    QPen pen(QBrush(Qt::black), penWidth);
    pen.setCapStyle(Qt::RoundCap);

    return pen;
}


void CurvedTreeGraphicsScene::setTreeLinePreferredX()
{
    int currentLineCount = getCurrentLineCount();

    double spacing = 1.0 / (currentLineCount + 1);
    double firstSpace;
    if (spacing > g_settings->maxSpacing)
    {
        spacing = g_settings->maxSpacing;
        firstSpace = (1.0 - spacing * (currentLineCount - 1)) / 2.0;
    }
    else
        firstSpace = spacing;

    int currentLineNumber = 0;
    for (std::list<CurvedTreeLine *>::iterator i = m_lines.begin();
         i != m_lines.end(); ++i)
    {
        if ((*i)->m_current)
        {
            (*i)->m_preferredX = firstSpace + spacing * currentLineNumber;
            ++currentLineNumber;
        }
    }
}


int CurvedTreeGraphicsScene::getCurrentLineCount()
{
    int currentLineCount = 0;
    for (std::list<CurvedTreeLine *>::iterator i = m_lines.begin();
         i != m_lines.end(); ++i)
    {
        if ((*i)->m_current)
            ++currentLineCount;
    }
    return currentLineCount;
}



void CurvedTreeGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    mousePressOrMove(event);
    QGraphicsScene::mousePressEvent(event);
}

void CurvedTreeGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->buttons() != Qt::NoButton)
        mousePressOrMove(event);
    QGraphicsScene::mouseMoveEvent(event);
}

void CurvedTreeGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    stopHighlighting();
    clearLandscapeThumbnail();
    g_mainWindow->showHistoryTip();
    QGraphicsScene::mouseReleaseEvent(event);
}

void CurvedTreeGraphicsScene::mousePressOrMove(QGraphicsSceneMouseEvent * event)
{
    double x = event->scenePos().x();
    double y = event->scenePos().y();

    if (y >= 0.0 && y <= height())
    {
        g_mainWindow->hideHistoryTip();
        highlight(x, y);
    }
    else
    {
        stopHighlighting();
        clearLandscapeThumbnail();
        g_mainWindow->showHistoryTip();
    }

    update();
}



void CurvedTreeGraphicsScene::highlight(double x, double y)
{
    //Look for a line close to the mouse position.
    //If multiple lines are found, choose the closest.
    CurvedTreeLine * closestLine = 0;
    double closestDistance = std::numeric_limits<double>::max();
    for (std::list<CurvedTreeLine *>::iterator i = m_lines.begin(); i != m_lines.end(); ++i)
    {
        (*i)->setZValue(0.0);

        double distance;
        if ((*i)->containsPoint(x, y, g_settings->treeLineSelectionMargin, &distance))
        {
            if (distance < closestDistance)
            {
                closestLine = *i;
                closestDistance = distance;
            }
        }
    }

    //If no line was found, clear the highlighting.
    if (closestLine == 0)
    {
        stopHighlighting();
        return;
    }

    //Bring the closest line to the top, so its highlighting won't
    //be obscured by other lines.
    closestLine->setZValue(1.0);

    //Find the history data point that best matches the selection location.
    double dateOfMousePosition = m_lastLoggedDateInTree * (1.0 - (y / height()));
    if (dateOfMousePosition < closestLine->m_startDate)
        dateOfMousePosition = closestLine->m_startDate;
    if (dateOfMousePosition > closestLine->m_endDate)
        dateOfMousePosition = closestLine->m_endDate;

    boost::shared_ptr<HistoryDataPoint> closestHistoryPoint = g_history->getClosestHistoryPoint(dateOfMousePosition);
    long long closestDate = closestHistoryPoint->m_date;

    //Now send the history landscape data to the widget that will show it.
    g_mainWindow->m_landscapeHistoryWidget->m_landscapeThumbnail = &closestHistoryPoint->m_landscapeThumbnail;
    g_mainWindow->m_landscapeHistoryWidget->update();
    g_mainWindow->setHistoryDate(closestDate);




    //If the selection is different from the current one, set up highlighting!
    if (closestLine != m_highlightedLine || closestDate != m_selectionDate)
    {
        //Determine the y coordinate associated with the selection date
        int steps = closestDate / g_settings->speciesCheckInterval;
        double dotY = height() - steps * g_settings->treeStepHeight;

        //First, highlight the line itself
        if (m_highlightedLine != 0)
            m_highlightedLine->unhighlight();

        m_highlightedLine = closestLine;
        m_selectionDate = closestDate;

        closestLine->highlight(PARTIAL_LINE, dotY);
        updateTree();

        if (g_settings->simulationShown)
            m_landscapeView->viewport()->update();

        //Now create a selection dot at the date nearest the mouse
        double dotX = closestLine->getX(steps);
        m_selectionDot.reset(new QGraphicsEllipseItem(dotX - g_settings->treeSelectionDotDiameter / 2.0,
                                                      dotY - g_settings->treeSelectionDotDiameter / 2.0,
                                                      g_settings->treeSelectionDotDiameter,
                                                      g_settings->treeSelectionDotDiameter));
        m_selectionDot->setPen(Qt::NoPen);
        m_selectionDot->setBrush(Qt::red);
        m_selectionDot->setZValue(2.0);
        addItem(m_selectionDot.get());

        //Now send the history organism to the widget that will show it.
        int speciesNumber = closestLine->m_species->m_speciesNumber;
        Organism * historyOrganism = closestHistoryPoint->m_randomOrganismForEachSpecies[speciesNumber];
        g_mainWindow->m_historyOrganismWidget->m_organism = historyOrganism;
        g_mainWindow->m_historyOrganismWidget->update();
        g_mainWindow->setHistorySpecies(speciesNumber);

        //Finally, give the species positions to be displayed on the landscape thumbnail
        g_mainWindow->m_landscapeHistoryWidget->m_organismPositions = &(closestHistoryPoint->m_organismLocationsForEachSpecies[speciesNumber]);
    }

    //If a line was found but it's the same line and at the same date as
    //the currently highlighted one, then there's nothing to do.
}


void CurvedTreeGraphicsScene::stopHighlighting()
{
    if (m_highlightedLine != 0)
    {
        m_highlightedLine->unhighlight();
        m_highlightedLine = 0;
        updateTree();

        if (g_settings->simulationShown)
            m_landscapeView->viewport()->update();
    }

    m_selectionDot.reset();

    if (g_mainWindow->m_historyOrganismWidget->m_organism != 0)
    {
        g_mainWindow->m_historyOrganismWidget->m_organism = 0;
        g_mainWindow->m_historyOrganismWidget->update();
    }

    if (g_mainWindow->m_landscapeHistoryWidget->m_organismPositions != 0)
    {
        g_mainWindow->m_landscapeHistoryWidget->m_organismPositions = 0;
        g_mainWindow->m_landscapeHistoryWidget->update();
    }

    m_selectionDate = -1; //indicates no selected date
    g_mainWindow->clearHistorySpecies();
}

void CurvedTreeGraphicsScene::clearLandscapeThumbnail()
{
    if (g_mainWindow->m_landscapeHistoryWidget->m_landscapeThumbnail != 0 ||
            g_mainWindow->m_landscapeHistoryWidget->m_organismPositions != 0)
    {
        g_mainWindow->m_landscapeHistoryWidget->m_landscapeThumbnail = 0;
        g_mainWindow->m_landscapeHistoryWidget->m_organismPositions = 0;
        g_mainWindow->m_landscapeHistoryWidget->update();
    }

    m_selectionDate = -1;
    g_mainWindow->clearHistoryDate();
}


void CurvedTreeGraphicsScene::rebuildTree()
{
    start(g_landscape->m_startingSpecies);

    int steps = g_landscape->m_elapsedTime / g_settings->speciesCheckInterval;

    for (int i = 1; i <= steps; ++i)
    {
        long long date = i * g_settings->speciesCheckInterval;
        growTreeOneStep(date);
    }

    updateTree();
}
