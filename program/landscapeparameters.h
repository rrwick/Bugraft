#ifndef LANDSCAPEPARAMETERS_H
#define LANDSCAPEPARAMETERS_H

#ifndef Q_MOC_RUN
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#endif // Q_MOC_RUN
namespace boost {namespace serialization {class access;}}

class LandscapeParameters
{
public:
    LandscapeParameters() {}
    LandscapeParameters(double perlinFrequency, double perlinPersistance,
                        double perlinZ, double waterLevel) :
        m_perlinFrequency(perlinFrequency), m_perlinPersistance(perlinPersistance),
        m_perlinZ(perlinZ), m_waterLevel(waterLevel)
    {}

    double m_perlinFrequency;
    double m_perlinPersistance;
    double m_perlinZ;
    double m_waterLevel;

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive & ar, const unsigned)
    {
        ar & m_perlinFrequency;
        ar & m_perlinPersistance;
        ar & m_perlinZ;
        ar & m_waterLevel;
    }
};

#endif // LANDSCAPEPARAMETERS_H
