#ifndef ORGANISM_H
#define ORGANISM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QRectF>
#include <QGraphicsSceneMouseEvent>

#include "genome.h"
#include "species.h"

#ifndef Q_MOC_RUN
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/serialization/shared_ptr.hpp"
#endif // Q_MOC_RUN
namespace boost {namespace serialization {class access;}}

class Landscape;

class Organism : public QGraphicsItem
{
public:
    Organism();
    Organism(double x, double y,
             boost::shared_ptr<Genome> parent1,
             boost::shared_ptr<Genome> parent2,
             boost::shared_ptr<Species> species);
    Organism(double x, double y,
             boost::shared_ptr<Genome> genome,
             boost::shared_ptr<Species> species);
    Organism(boost::shared_ptr<Genome> genome);
    ~Organism();

    double m_x, m_y;
    int m_timeSinceLastBump;
    boost::shared_ptr<Genome> m_genome;
    boost::shared_ptr<Genome> m_fertiliserGenome;
    boost::shared_ptr<Species> m_species;
    bool m_historyOrganism;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    bool hasDied() const;
    void move();
    void setPosition() {setPos(m_x, m_y);}
    void checkForCollisions();
    void clearCollisionVector();
    double calculateDistance(Organism * other) {return m_genome->calculateDistance(other->m_genome);}
    int getAge() const;
    bool isHistoryOrganism() {return m_historyOrganism;}

private:
    double m_movementAngle;
    double m_orientationAngle;
    long long m_birthDate;
    long long m_deathDate;
    bool m_drifting;
    bool m_dead;
    bool m_startingOrganism;

    double getScale() const;
    void commonConstructorCode();

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);


    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive & ar, const unsigned)
    {
        ar & m_x;
        ar & m_y;
        ar & m_timeSinceLastBump;
        ar & m_genome;
        ar & m_fertiliserGenome;
        ar & m_species;
        ar & m_historyOrganism;
        ar & m_movementAngle;
        ar & m_orientationAngle;
        ar & m_birthDate;
        ar & m_deathDate;
        ar & m_drifting;
        ar & m_dead;
        ar & m_startingOrganism;

        if (isHistoryOrganism())
            ++g_historyOrganismsSavedOrLoaded;
        else
            ++g_organismsSavedOrLoaded;
    }
};

#endif // ORGANISM_H
