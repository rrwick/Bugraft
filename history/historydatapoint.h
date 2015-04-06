#ifndef HISTORYDATAPOINT_H
#define HISTORYDATAPOINT_H

#include <QPixmap>
#include <map>
#include <vector>
#include <QPointF>
#include <utility> //std::pair
#include "../program/landscapeparameters.h"

#ifndef Q_MOC_RUN
#include "boost/serialization/map.hpp"
#include "boost/serialization/utility.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#endif // Q_MOC_RUN
namespace boost {namespace serialization {class access;}}

class Organism;
class Genome;

class HistoryDataPoint
{
public:
    HistoryDataPoint() : m_landscapeThumbnail(QPixmap()){}
    HistoryDataPoint(long long date,
                     std::map<int, boost::shared_ptr<Genome> > randomGenomeForEachSpecies,
                     LandscapeParameters landscapeParameters,
                     std::map<int, std::vector< std::pair<float, float> > > organismLocationsForEachSpecies);
    ~HistoryDataPoint();

    long long m_date;
    LandscapeParameters m_landscapeParameters;
    QPixmap m_landscapeThumbnail;
    std::map<int, Organism *> m_randomOrganismForEachSpecies;
    std::map<int, std::vector< std::pair<float, float> > > m_organismLocationsForEachSpecies;

    void makeThumbnail();

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive & ar, const unsigned)
    {
        ar & m_date;
        ar & m_landscapeParameters;
        ar & m_randomOrganismForEachSpecies;
        ar & m_organismLocationsForEachSpecies;
    }
};

#endif // HISTORYDATAPOINT_H
