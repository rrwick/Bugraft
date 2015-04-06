#include "saverandloader.h"

#include "landscape.h"
#include "../history/history.h"
#include "../ui/mainwindow.h"
#include "globals.h"
#include "settings.h"

#include <fstream>
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/filter/gzip.hpp"


void SaverAndLoader::saveSimulation()
{
    std::ofstream ofs(m_fullFileName.toLocal8Bit().data(), std::ios_base::out | std::ios::binary);
    boost::iostreams::filtering_ostream out;
    out.push(boost::iostreams::gzip_compressor(1)); //1 is the compression level - I chose a low one for speed.
    out.push(ofs);
    boost::archive::text_oarchive ar(out);

    ar << *g_landscape << *g_history << *g_settings;

    emit finishedSaving();
}

void SaverAndLoader::loadSimulation()
{
    std::ifstream ifs(m_fullFileName.toLocal8Bit().data(), std::ios_base::in | std::ios_base::binary);
    boost::iostreams::filtering_istream in;
    in.push(boost::iostreams::gzip_decompressor());
    in.push(ifs);
    boost::archive::text_iarchive ar(in);

    ar >> *g_landscape >> *g_history >> *g_settings;

    g_landscape->rebuildAfterLoad();
    g_history->setMaxBoundingBoxAfterLoad();
    g_history->remakeAllHistoryThumbnails();

    emit finishedLoading();
}


