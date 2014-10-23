#ifndef WORLD_GEOGRAPHY_HPP
#define WORLD_GEOGRAPHY_HPP

#include <object/object.hpp>
#include <object/structure.hpp>

#include <boost/polygon/point_data.hpp>
#include <boost/polygon/segment_data.hpp>

#include <boost/random/mersenne_twister.hpp>

namespace world {

typedef boost::polygon::point_data<double> coord;
typedef boost::polygon::segment_data<double> segment;

struct physical_characteristics : public object::property {
    coord centre;
    double size_bias;
};

class geography {
private:
    object::store &m_store;
    object::structure<physical_characteristics> m_physicals;
    std::vector<object::ref> m_regions;
    boost::random::mt19937 m_gen;
public:
    geography(object::store &store) : m_store(store), m_physicals(store) {}

    void generate();
private:
    std::vector<std::vector<segment>> generate_voronoi();
};

} // namespace world

#endif
