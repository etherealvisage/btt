#include <iostream>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>

#include <boost/polygon/voronoi.hpp>

#include "geography.hpp"

namespace world {

void geography::generate() {
    /* Make random list of regions. */
    boost::random::uniform_int_distribution<> rcdist(5,100);
    int region_count = rcdist(m_gen);
    while(region_count-- > 0) m_regions.push_back(m_store.make());

    /* Fill in centre and size biases. */
    boost::random::uniform_real_distribution<> cdist(0.0,1.0);
    boost::random::normal_distribution<> bdist(1.0);

    for(auto region : m_regions) {
        physical_characteristics pc;
        pc.centre = coord(cdist(m_gen), cdist(m_gen));
        pc.size_bias = std::min(0.01, bdist(m_gen));
        region->set(pc);
    }

    /* Generate Voronoi diagram. */
    auto result = generate_voronoi();
}

std::vector<std::vector<segment>> geography::generate_voronoi() {
    std::vector<coord> vpts;
    // TODO: get number of map points from somewhere else.
    const int vpts_count = 100000;

    boost::random::uniform_real_distribution<> vdist(0.0,1.0);
    for(int i = 0; i < vpts_count; i ++) {
        vpts.push_back(coord(vdist(m_gen), vdist(m_gen)));
    }

    // NOTE: not actually using any segments.
    std::vector<segment> vsegs;

    boost::polygon::voronoi_diagram<double> vd;
    boost::polygon::construct_voronoi(vpts.begin(), vpts.end(),
        vsegs.begin(), vsegs.end(), &vd);
    std::vector<std::vector<segment>> results;

    

    return results;
}

} // namespace world
