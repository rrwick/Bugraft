#ifndef HISTORY_H
#define HISTORY_H

#include "historydatapoint.h"
#include <vector>
#include <QRectF>

#ifndef Q_MOC_RUN
#include "boost/serialization/vector.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/serialization/shared_ptr.hpp"
#endif // Q_MOC_RUN
namespace boost {namespace serialization {class access;}}

class Landscape;

class History
{
public:
    History();
    ~History();

    std::vector<boost::shared_ptr<HistoryDataPoint>> m_historyPoints;
    QRectF m_maxBoundingBox;

    void logHistory();
    void reset();
    void setMaxBoundingBoxAfterLoad();
    void remakeAllHistoryThumbnails();
    void possiblyEnlargeMaxBoundingBox(QRectF newBox);
    long long getClosestLoggedDate(double date);
    boost::shared_ptr<HistoryDataPoint> getClosestHistoryPoint(double date);

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive & ar, const unsigned)
    {
        ar & m_historyPoints;
    }
};

#endif // HISTORY_H
