#include "squaretreegraphicsscene.h"
#include "../../program/settings.h"
#include "../../program/globals.h"
#include <QBrush>
#include "../../program/landscape.h"
#include "../../history/history.h"
#include "../../ui/mainwindow.h"


SquareTreeGraphicsScene::SquareTreeGraphicsScene(QGraphicsView * landscapeView) :
    m_height(0),
    m_width(g_settings->treeWidgetMinimumWidth),
    m_treePen(getTreePen()),
    m_spacing(0.5),
    m_landscapeView(landscapeView),
    m_lastLoggedDateInTree(0),
    m_selectionDot(),
    m_startingSpecies(0),
    m_selectionDate(-1),
    m_stepCount(0),
    m_highlightedLine(0)
{

}

SquareTreeGraphicsScene::~SquareTreeGraphicsScene()
{
    cleanUp();
}

void SquareTreeGraphicsScene::cleanUp()
{
    clear();
    m_speciesLines.clear();
    m_horizontalLines.clear();
}

void SquareTreeGraphicsScene::start(boost::shared_ptr<Species> startingSpecies)
{
    cleanUp();

    m_stepCount = 0;
    m_startingSpecies = startingSpecies;
    m_lastLoggedDateInTree = 0;

    SquareTreeSpeciesLine * firstLine = new SquareTreeSpeciesLine(startingSpecies, 0.5, m_stepCount, height(), m_width, getTreePen());

    addItem(firstLine);
    m_speciesLines.push_back(firstLine);
}

void SquareTreeGraphicsScene::growTreeOneStep(long long date)
{
    ++m_stepCount;
    m_lastLoggedDateInTree = date;
    bool newLines = false;

    for (std::list<SquareTreeSpeciesLine *>::iterator i = m_speciesLines.begin(); i != m_speciesLines.end(); ++i)
    {
        SquareTreeSpeciesLine * line = *i;

        //Nothing to be done here for lines that are SPLIT or EXTINCT
        if (line->m_state != CURRENT)
            continue;

        line->m_endStep = m_stepCount;

        //If the species is current, there's nothing more to be done.
        if (line->m_species->getStateAtDate(date) == CURRENT) {}

        //If the species has gone extinct, simply label the line as extinct.
        //This will make it ignored in future calls of this function.
        else if (line->m_species->getStateAtDate(date) == EXTINCT)
            line->m_state = EXTINCT;


        //If the species has split, then there is much to be done.  Child
        //lines have to be made and inserted into the appropriate positions.
        else // line->m_species->m_state == SPLIT
        {
            line->m_state = SPLIT;
            QPen pen = getTreePen();

            int childSpeciesCount = int(line->m_species->m_childSpecies.size());
            std::vector<SquareTreeSpeciesLine *> branches;
            for (int k = 0; k < childSpeciesCount; ++k)
            {
                boost::shared_ptr<Species> child = line->m_species->m_childSpecies[k];
                SquareTreeSpeciesLine * newLine = new SquareTreeSpeciesLine(child, line->m_xFraction,
                                                                m_stepCount, height(), m_width, pen);

                addItem(newLine);
                m_speciesLines.insert(i, newLine);
                branches.push_back(newLine);
            }

            //Make the horizontal line
            SquareTreeHorizontalLine * newHorizontalLine = new SquareTreeHorizontalLine(branches, height(), m_width, pen);
            line->m_treeHorizontalLine = newHorizontalLine;
            m_horizontalLines.push_back(newHorizontalLine);
            addItem(newHorizontalLine);

            newLines = true;
        }
    }

    //If new lines were added, update the x axis positions of the lines.
    if (newLines)
        updateXPositions();

    //If something was highlighted in the scene (i.e. the mouse was down),
    //it is necessary to shift some positions downward.
    if (m_selectionDot != NULL)
        m_selectionDot->moveBy(0, g_settings->treeStepHeight);
    if (m_highlightedLine != 0)
        m_highlightedLine->m_highlightedY += g_settings->treeStepHeight;
}


//Leaf nodes (current and extinct species) are spaced evenly from
//left to right.  Split nodes are spaced in the middle of their children.
void SquareTreeGraphicsScene::updateXPositions()
{
    int numberOfLeafNodes = 0;
    for (std::list<SquareTreeSpeciesLine *>::iterator i = m_speciesLines.begin(); i != m_speciesLines.end(); ++i)
    {
        if ((*i)->isLeafNode())
            ++numberOfLeafNodes;
    }
    m_spacing = 1.0 / (numberOfLeafNodes + 1);

    //First go through all nodes, setting leaf node x positions
    int leafNodeNumber = 1;
    for (std::list<SquareTreeSpeciesLine *>::iterator i = m_speciesLines.begin(); i != m_speciesLines.end(); ++i)
    {
        if ((*i)->isLeafNode())
        {
            (*i)->m_xFraction = leafNodeNumber * m_spacing;
            ++leafNodeNumber;
        }
    }

    //Now that leaf x positions are set, use them to set split node x positions
    for (std::list<SquareTreeSpeciesLine *>::iterator i = m_speciesLines.begin(); i != m_speciesLines.end(); ++i)
    {
        if ((*i)->isNotLeafNode())
        {
            boost::shared_ptr<Species> firstChild = (*i)->m_species->m_childSpecies.front();
            boost::shared_ptr<Species> lastChild = (*i)->m_species->m_childSpecies.back();
            double leftX = firstChild->m_squareTreeSpeciesLine->m_xFraction;
            double rightX = lastChild->m_squareTreeSpeciesLine->m_xFraction;

            (*i)->m_xFraction = (leftX + rightX) / 2.0;
        }
    }
}

void SquareTreeGraphicsScene::updateTree()
{
    m_height = m_stepCount * g_settings->treeStepHeight;
    setSceneRect(0, 0, m_width, m_height);
    QPen pen = getTreePen();

    for (std::list<SquareTreeHorizontalLine *>::iterator i = m_horizontalLines.begin(); i != m_horizontalLines.end(); ++i)
        (*i)->updateHorizontalLine(m_height, m_width, pen);

    for (std::list<SquareTreeSpeciesLine *>::iterator i = m_speciesLines.begin(); i != m_speciesLines.end(); ++i)
        (*i)->updateSpeciesLine(m_height, m_width, pen);
}

QPen SquareTreeGraphicsScene::getTreePen()
{
    double penWidth = g_settings->treeLineThickness;
    double widthOverMinimum = m_width - g_settings->treeWidgetMinimumWidth;
    penWidth += g_settings->treeLineThickness * widthOverMinimum / g_settings->treeWidgetMinimumWidth / 3.0;

    QPen pen(QBrush(Qt::black), penWidth);
    pen.setCapStyle(Qt::RoundCap);

    return pen;
}


void SquareTreeGraphicsScene::highlight(double x, double y)
{
    //Find the history data point that best matches the selection location.
    double dateOfMousePosition = m_lastLoggedDateInTree * (1.0 - (y / height()));
    boost::shared_ptr<HistoryDataPoint> closestHistoryPoint = g_history->getClosestHistoryPoint(dateOfMousePosition);
    long long closestDate = closestHistoryPoint->m_date;

    //Now send the history landscape data to the widget that will show it.
    g_mainWindow->m_landscapeHistoryWidget->m_landscapeThumbnail = &closestHistoryPoint->m_landscapeThumbnail;
    g_mainWindow->m_landscapeHistoryWidget->update();
    g_mainWindow->setHistoryDate(closestDate);


    //Look for a line close to the mouse position.
    //If multiple lines are found, choose the closest.
    SquareTreeSpeciesLine * closestLine = 0;
    double closestDistance = std::numeric_limits<double>::max();
    for (std::list<SquareTreeSpeciesLine *>::iterator i = m_speciesLines.begin(); i != m_speciesLines.end(); ++i)
    {
        double xDistance;
        if ((*i)->containsPoint(x, y, g_settings->treeLineSelectionMargin, &xDistance))
        {
            if (xDistance < closestDistance)
            {
                closestLine = *i;
                closestDistance = xDistance;
            }
        }
    }

    //If no line was found, clear the highlighting.
    if (closestLine == 0)
    {
        stopHighlighting();
        return;
    }

    //If the selection is different from the current one, set up highlighting!
    if (closestLine != m_highlightedLine || closestDate != m_selectionDate)
    {
        //If there are no organisms at that point for that species, quit.
        //This is likely to happen at the very end of a species line,
        //because at that date it will have split into child species.
        int speciesNumber = closestLine->m_species->m_speciesNumber;
        if (closestHistoryPoint->m_organismLocationsForEachSpecies[speciesNumber].size() == 0)
        {
            stopHighlighting();
            return;
        }

        //Determine the y coordinate associated with the selection date
        double steps = double(closestDate) / g_settings->speciesCheckInterval;
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
        m_selectionDot.reset(new QGraphicsEllipseItem(closestLine->m_lineX - g_settings->treeSelectionDotDiameter / 2.0,
                                                      dotY - g_settings->treeSelectionDotDiameter / 2.0,
                                                      g_settings->treeSelectionDotDiameter,
                                                      g_settings->treeSelectionDotDiameter));
        m_selectionDot->setPen(Qt::NoPen);
        m_selectionDot->setBrush(Qt::red);
        addItem(m_selectionDot.get());

        //Now send the history organism to the widget that will show it.
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

void SquareTreeGraphicsScene::stopHighlighting()
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

void SquareTreeGraphicsScene::clearLandscapeThumbnail()
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


void SquareTreeGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    mousePressOrMove(event);
    QGraphicsScene::mousePressEvent(event);
}

void SquareTreeGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->buttons() != Qt::NoButton)
        mousePressOrMove(event);
    QGraphicsScene::mouseMoveEvent(event);
}

void SquareTreeGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    stopHighlighting();
    clearLandscapeThumbnail();
    g_mainWindow->showHistoryTip();
    QGraphicsScene::mouseReleaseEvent(event);
}

void SquareTreeGraphicsScene::mousePressOrMove(QGraphicsSceneMouseEvent * event)
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


void SquareTreeGraphicsScene::rebuildTree()
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
