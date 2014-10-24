#ifndef WORLD_GEOGRAPHY_HPP
#define WORLD_GEOGRAPHY_HPP

#include <object/object.hpp>
#include <object/structure.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <boost/random/mersenne_twister.hpp>

namespace world {

typedef boost::geometry::model::d2::point_xy<double> coord_t;
typedef boost::geometry::model::polygon<coord_t> polygon_t;

struct physical_characteristics : public object::property {
    coord_t centre;
    double size_bias;
    polygon_t borders;
};

class geography {
private:
    struct vregion {
        std::set<vregion *> neighbours;
        polygon_t outline;
        int assignment;
    };
private:
    object::store &m_store;
    object::structure<physical_characteristics> m_physicals;
    std::vector<object::ref> m_regions;
    boost::random::mt19937 m_gen;
public:
    geography(object::store &store) : m_store(store), m_physicals(store) {}

    void generate();
private:
    std::vector<vregion> generate_voronoi();
};

} // namespace world

#endif
