#ifndef SPECIES_H
#define SPECIES_H

#include <vector>
#include "../program/globals.h"


#ifndef Q_MOC_RUN
#include "boost/serialization/vector.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/serialization/shared_ptr.hpp"
#endif // Q_MOC_RUN
namespace boost {namespace serialization {class access;}}

class SquareTreeSpeciesLine;
class CurvedTreeLine;

class Species
{
public:
    Species() : m_squareTreeSpeciesLine(0), m_curvedTreeLine(0) {}
    Species(boost::shared_ptr<Species> parentSpecies, long long date);

    int m_speciesNumber;
    static int m_highestSpeciesNumber;
    SpeciesState m_state;
    long long m_startDate;
    long long m_endDate;
    boost::shared_ptr<Species> m_parentSpecies;
    std::vector<boost::shared_ptr<Species>> m_childSpecies;
    SquareTreeSpeciesLine * m_squareTreeSpeciesLine;
    CurvedTreeLine * m_curvedTreeLine;

    QString getNewickTree(long long date);
    SpeciesState getStateAtDate(long long date);

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive & ar, const unsigned)
    {
        ar & m_speciesNumber;
        ar & m_highestSpeciesNumber;
        ar & m_state;
        ar & m_startDate;
        ar & m_endDate;
        ar & m_parentSpecies;
        ar & m_childSpecies;
    }
};

#endif // SPECIES_H
