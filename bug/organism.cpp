#include "organism.h"

#include "../program/globals.h"
#include "../program/randomnumbers.h"
#include "../program/settings.h"
#include "math.h"
#include "../program/landscape.h"
#include <QPainterPath>
#include <QMessageBox>
#include "../ui/mainwindow.h"
#include "../ui/organisminfodialog.h"
#include "../tree/square/squaretreespeciesline.h"


Organism::Organism() :
    m_timeSinceLastBump(0),
    m_genome(),
    m_fertiliserGenome(),
    m_species(),
    m_historyOrganism(false),
    m_birthDate(0),
    m_drifting(false),
    m_dead(false),
    m_startingOrganism(false)
{
}

//This constructor makes most organisms - ones with two parents in the
//simulation.
Organism::Organism(double x, double y,
                   boost::shared_ptr<Genome> parent1,
                   boost::shared_ptr<Genome> parent2,
                   boost::shared_ptr<Species> species) :
    QGraphicsItem(),
    m_x(x), m_y(y),
    m_timeSinceLastBump(0),
    m_genome(new Genome(parent1, parent2)),
    m_fertiliserGenome(),
    m_species(species),
    m_historyOrganism(false),
    m_birthDate(g_landscape->m_elapsedTime),
    m_drifting(false),
    m_dead(false),
    m_startingOrganism(false)
{
    commonConstructorCode();
}


//This constructor makes the initial batch of organisms.  It takes a
//genome, which will be the genome for all starting organisms, and it
//creates them with a positive age, so they are drawn either fully grown
//or partially grown.
Organism::Organism(double x, double y,
                   boost::shared_ptr<Genome> genome,
                   boost::shared_ptr<Species> species) :
    QGraphicsItem(),
    m_x(x), m_y(y),
    m_timeSinceLastBump(0),
    m_genome(genome),
    m_fertiliserGenome(),
    m_species(species),
    m_historyOrganism(false),
    m_drifting(false),
    m_dead(false),
    m_startingOrganism(true)
{
    int age = g_randomNumbers->getRandomInt(g_settings->organismFullyGrownAge / 3,
                                            3 * g_settings->organismFullyGrownAge);
    m_birthDate = g_landscape->m_elapsedTime - age;

    commonConstructorCode();
}


//This constructor makes an organism for the history, not for the actual
//simulation.
Organism::Organism(boost::shared_ptr<Genome> genome) :
    QGraphicsItem(),
    m_x(0.0), m_y(0.0),
    m_timeSinceLastBump(0),
    m_genome(genome),
    m_fertiliserGenome(0),
    m_species(),
    m_historyOrganism(true),
    m_movementAngle(0.0),
    m_orientationAngle(1.570796327),
    m_birthDate(0),
    m_deathDate(0),
    m_drifting(false),
    m_dead(false),
    m_startingOrganism(false)
{
}


void Organism::commonConstructorCode()
{
    m_movementAngle = g_randomNumbers->getRandomAngleRadians();
    m_orientationAngle = m_movementAngle;
    double lifeSpan = g_randomNumbers->getRandomOrganismLifespan();
    m_deathDate = g_landscape->m_elapsedTime + lifeSpan;
    setFlag(QGraphicsItem::ItemIsMovable);
}

Organism::~Organism()
{
}




void Organism::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    //Test code to make sure the bounding rect is okay
//    QPen boxPen(Qt::black);
//    boxPen.setWidth(0.2);
//    painter->setPen(boxPen);
//    painter->drawRect(boundingRect());

    //The walkState variable makes the organisms leg angles change so
    //they appear to be walking.
    //The organism age is used in the calculation so all organisms
    //are not walking in step.
    int walkState;
    if (m_historyOrganism)
        walkState = 0;
    else
    {
        walkState = (getAge()) % 4 - 1;
        if (walkState == 2)
            walkState = 0;
    }

    //History organisms don't have a species, and they won't be highlighted.
    bool highlighted;
    if (m_species == 0)
        highlighted = false;
    else if (m_species->m_squareTreeSpeciesLine == 0 || m_species->m_curvedTreeLine == 0)
        highlighted = false;
    else
        highlighted = (m_species->m_squareTreeSpeciesLine->m_highlightState == WHOLE_LINE ||
                       m_species->m_squareTreeSpeciesLine->m_highlightState == PARTIAL_LINE ||
                       m_species->m_curvedTreeLine->m_highlightState == WHOLE_LINE ||
                       m_species->m_curvedTreeLine->m_highlightState == PARTIAL_LINE);

    double scale = getScale();

    painter->rotate(-m_orientationAngle * 57.295779513 + 90);
    painter->scale(scale, scale);

    double width = m_genome->getGene(0) * 8.0 + 20.0;
    double length = m_genome->getGene(1) * 8.0 + 60.0;

    double top = -length / 2.0;
    double bottom = length / 2.0;
    double left = -width / 2.0;
    double right = width / 2.0;

    double red = m_genome->getGene(2) * 5.0;
    double green = m_genome->getGene(3) * 5.0;
    double blue = m_genome->getGene(4) * 5.0;
    double frontBackSkew = m_genome->getGene(5) / 60.0 + 0.125;

    double sidePointY = (top - bottom) * frontBackSkew + bottom;

    double topControlPointSpread = m_genome->getGene(6) * width / 100.0 + 20.0;
    double bottomControlPointSpread = m_genome->getGene(7) * width / 100.0 + 20.0;
    double sideControlTopPointSpread = m_genome->getGene(8) * (top - sidePointY) / 100.0 - 25.0;
    double sideControlBottomSpread = m_genome->getGene(9) * (sidePointY - bottom) / 100.0 - 25.0;


    QPointF topPoint(0.0, top);
    QPointF bottomPoint(0.0, bottom);
    QPointF leftPoint(left, sidePointY);
    QPointF rightPoint(right, sidePointY);

    QPointF topControlPointLeft(-topControlPointSpread, top);
    QPointF topControlPointRight(topControlPointSpread, top);
    QPointF bottomControlPointLeft(-bottomControlPointSpread, bottom);
    QPointF bottomControlPointRight(bottomControlPointSpread, bottom);
    QPointF leftControlPointTop(left, sidePointY + sideControlTopPointSpread);
    QPointF rightControlPointTop(right, sidePointY + sideControlTopPointSpread);
    QPointF leftControlPointBottom(left, sidePointY - sideControlBottomSpread);
    QPointF rightControlPointBottom(right, sidePointY - sideControlBottomSpread);

    QPainterPath bodyPath;
    bodyPath.moveTo(leftPoint);
    bodyPath.cubicTo(leftControlPointTop, topControlPointLeft, topPoint);
    bodyPath.cubicTo(topControlPointRight, rightControlPointTop, rightPoint);
    bodyPath.cubicTo(rightControlPointBottom, bottomControlPointRight, bottomPoint);
    bodyPath.cubicTo(bottomControlPointLeft, leftControlPointBottom, leftPoint);


    double eyeY = -length / 4.0;
    double eyeSize = m_genome->getGene(10) * width / 250.0 + 5.0;
    double eyeSpacing = m_genome->getGene(11) * width / 200.0 + eyeSize;
    double leftEyeX = -eyeSpacing;
    double rightEyeX = eyeSpacing;

    QPointF leftEyeCentre = QPointF(leftEyeX, eyeY);
    QPointF rightEyeCentre = QPointF(rightEyeX, eyeY);

    double legLength = m_genome->getGene(12) * 2 + width / 2.0 + 5.0;
    double legAngle = m_genome->getGene(13) / 75.0;
    double rightLegAngle = legAngle + walkState * g_settings->walkingLegAngle;
    double leftLegAngle = legAngle - walkState * g_settings->walkingLegAngle;
    double legSpacing = m_genome->getGene(14) * length / 250.0 + 3.0;
    double legThickness = m_genome->getGene(15) / 2.0 + 3.0;

    QPointF frontLegsStart = QPointF(0.0, -legSpacing);
    QPointF backLegsStart = QPointF(0.0, legSpacing);

    double frontLeftLegAngle = 3.14159265 - leftLegAngle;
    double frontRightLegAngle = rightLegAngle;
    double backLeftLegAngle = 3.14159265 + leftLegAngle;
    double backRightLegAngle = -rightLegAngle;

    QPointF frontLeftLegEnd = QPointF(frontLegsStart.x() + cos(frontLeftLegAngle) * legLength,
                                      frontLegsStart.y() - sin(frontLeftLegAngle) * legLength);
    QPointF frontRightLegEnd = QPointF(frontLegsStart.x() + cos(frontRightLegAngle) * legLength,
                                      frontLegsStart.y() - sin(frontRightLegAngle) * legLength);
    QPointF backLeftLegEnd = QPointF(backLegsStart.x() + cos(backLeftLegAngle) * legLength,
                                      backLegsStart.y() - sin(backLeftLegAngle) * legLength);
    QPointF backRightLegEnd = QPointF(backLegsStart.x() + cos(backRightLegAngle) * legLength,
                                      backLegsStart.y() - sin(backRightLegAngle) * legLength);


    if (highlighted)
    {
        QPen highlightPen(Qt::red);
        highlightPen.setWidth(legThickness + 2 * g_settings->organismHighlightThickness);
        highlightPen.setCapStyle(Qt::RoundCap);
        painter->setPen(highlightPen);

        painter->drawLine(frontLegsStart, frontLeftLegEnd);
        painter->drawLine(frontLegsStart, frontRightLegEnd);
        painter->drawLine(backLegsStart, backLeftLegEnd);
        painter->drawLine(backLegsStart, backRightLegEnd);

        painter->drawPath(bodyPath);

    }


    QPen legPen(Qt::black);
    legPen.setWidth(legThickness);
    legPen.setCapStyle(Qt::RoundCap);
    painter->setPen(legPen);
    painter->drawLine(frontLegsStart, frontLeftLegEnd);
    painter->drawLine(frontLegsStart, frontRightLegEnd);
    painter->drawLine(backLegsStart, backLeftLegEnd);
    painter->drawLine(backLegsStart, backRightLegEnd);

    QBrush colorBrush(QColor(red, green, blue));
    painter->setBrush(colorBrush);
    QPen bodyPen(Qt::black);
    bodyPen.setWidth(8);
    painter->setPen(bodyPen);

    painter->drawPath(bodyPath);

    QBrush blackBrush(Qt::black);
    painter->setBrush(blackBrush);

    painter->drawEllipse(leftEyeCentre, eyeSize, eyeSize);
    painter->drawEllipse(rightEyeCentre, eyeSize, eyeSize);

}


//THIS FUNCTION IS A BIT CRUDE AT THE MOMENT!  MAYBE REWRITE LATER TO BE MORE PRECISE?
QRectF Organism::boundingRect() const
{
    double scale = getScale();

    double width = m_genome->getGene(0) * 8.0 + 20.0;
    double length = m_genome->getGene(1) * 8.0 + 60.0;
    double legLength = m_genome->getGene(12) * 2 + width / 2.0 + 5.0;

    double maxDimension = std::max(length, legLength * 2.0);
    double scaledBoxSize = maxDimension * scale * 1.2; //The 1.2 is just to add a bit for safety.
    double halfScaledBoxSize = scaledBoxSize / 2.0;

    return QRectF(-halfScaledBoxSize, -halfScaledBoxSize, scaledBoxSize, scaledBoxSize);
}

void Organism::move()
{
    double newX, newY;

    if (m_drifting)
    {
        newX = m_x + cos(m_movementAngle) * g_settings->organismDriftStepDistance;
        newY = m_y - sin(m_movementAngle) * g_settings->organismDriftStepDistance;

        //If it has drifted off the edge of the map, it dies.
        if (g_landscape->pointIsOutOfBounds(newX, newY))
        {
            m_dead = true;
            return;
        }

        m_movementAngle += g_randomNumbers->getRandomDouble(-g_settings->organismDriftingAngleChange,
                                                        g_settings->organismDriftingAngleChange);
        m_orientationAngle += g_settings->organismDriftingRotationRate;

        m_x = newX;
        m_y = newY;

        //If an organism has drifted to a habitable area, then it stops drifting.
        if (g_landscape->pointIsHabitable(m_x, m_y))
            m_drifting = false;

        return;
    }


    //If the code got here, then the organism is not drifting but moving normally.

    newX = m_x + cos(m_movementAngle) * g_settings->organismStepDistance;
    newY = m_y - sin(m_movementAngle) * g_settings->organismStepDistance;

    m_movementAngle += g_randomNumbers->getRandomDouble(-g_settings->organismAngleChange,
                                                    g_settings->organismAngleChange);

    //If the organism is currently in an uninhabitable place (perhaps got there due to
    //a change in water level), then it should die.
    if (g_landscape->pointIsNotHabitable(m_x, m_y))
        m_dead = true;

    GroundType groundType = g_landscape->getGroundType(newX, newY);

    //If the organism reached the edge, it will turn around.
    if (groundType == OUT_OF_BOUNDS)
        m_movementAngle += 3.14159265;

    //If the new destination is not habitable, there are two possibilties:
    // -the organism fails to move and turns around (most likely)
    // -the organism begins drifting (less likely)
    else if (groundType == WATER)
    {
        if (g_randomNumbers->chanceOfTrue(g_settings->chanceOfDriftingOverWater))
        {
            m_drifting = true;
            m_x = newX;
            m_y = newY;
        }
        else
            m_movementAngle += 3.14159265;
    }
    else if (groundType == HIGH_ALTITUDE)
    {
        if (g_randomNumbers->chanceOfTrue(g_settings->chanceOfDriftingOverAltitude))
        {
            m_drifting = true;
            m_x = newX;
            m_y = newY;
        }
        else
            m_movementAngle += 3.14159265;
    }

    //If the new destination is habitable, then the organism moves.
    else //groundType == HABITABLE
    {
        m_x = newX;
        m_y = newY;
    }

    m_orientationAngle = m_movementAngle;

    ++m_timeSinceLastBump;
    if (m_timeSinceLastBump > g_settings->bumpFreeTimeBeforeGivingBirth &&
            m_fertiliserGenome != NULL)
    {
        Organism * child;
        child = new Organism(m_x, m_y, m_genome, m_fertiliserGenome, m_species);
        g_landscape->addNewOrganism(child);
        m_timeSinceLastBump = 0;
    }

}

int Organism::getAge() const
{
    return g_landscape->m_elapsedTime - m_birthDate;
}

double Organism::getScale() const
{
    if (m_historyOrganism)
        return g_settings->organismScale;

    int age = getAge();
    double beforeGeneralScaling;

    if (age > g_settings->organismFullyGrownAge)
        beforeGeneralScaling = 1.0;
    else
        beforeGeneralScaling = double(age) / g_settings->organismFullyGrownAge;

     return beforeGeneralScaling * g_settings->organismScale;
}

bool Organism::hasDied() const
{
    return m_dead || g_landscape->m_elapsedTime >= m_deathDate;
}

void Organism::checkForCollisions()
{
    Organism * collidingOrganism = g_landscape->addToCollisionVector(m_x, m_y, this);

    if (collidingOrganism != 0)
    {
        m_timeSinceLastBump = 0;
        collidingOrganism->m_timeSinceLastBump = 0;

        //If the organisms are genetically similar enough, they fertilise each other here
        if (m_species->m_speciesNumber == collidingOrganism->m_species->m_speciesNumber &&
                m_genome->calculateDistance(collidingOrganism->m_genome) <= g_settings->maximumDistanceForMating)
        {
            m_fertiliserGenome.reset();
            collidingOrganism->m_fertiliserGenome.reset();

            m_fertiliserGenome = collidingOrganism->m_genome;
            collidingOrganism->m_fertiliserGenome = m_genome;
        }
    }
}
void Organism::clearCollisionVector()
{
    g_landscape->clearCollisionVector(m_x, m_y);
}



void Organism::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    QPointF difference = event->pos() - event->lastPos();
    m_x += difference.x();
    m_y += difference.y();

    QGraphicsItem::mouseMoveEvent(event);
}

void Organism::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->button() == Qt::RightButton && contains(event->pos()))
    {
        bool simulationRunning = g_mainWindow->simulationIsRunning();
        if (simulationRunning)
            g_mainWindow->stopSimulation();

        OrganismInfoDialog infoDialog(this, g_mainWindow);
        infoDialog.exec();

        if (simulationRunning)
            g_mainWindow->startSimulation();
    }


    QGraphicsItem::mouseReleaseEvent(event);
}
