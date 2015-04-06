#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <QString>

#ifndef Q_MOC_RUN
#include "boost/shared_ptr.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#endif // Q_MOC_RUN
namespace boost {namespace serialization {class access;}}

class Genome
{
public:
    Genome();
    Genome(boost::shared_ptr<Genome> parent1, boost::shared_ptr<Genome> parent2);
    Genome(std::vector<float> genes);

    float getGene(int i) const {return m_genes[i];}
    void randomise();
    float calculateDistance(boost::shared_ptr<Genome> other);
    static float calculateDistance(QString * genomeString1, QString * genomeString2);
    QString getString();
    void perhapsMutate();

private:
    std::vector<float> m_genes;

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive & ar, const unsigned)
    {
        ar & m_genes;
    }
};

#endif // GENOME_H
