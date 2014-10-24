#include <iostream>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>

#include <boost/polygon/voronoi.hpp>
#include <boost/polygon/point_data.hpp>
#include <boost/polygon/segment_data.hpp>

#include "geography.hpp"

namespace world {

void geography::generate() {
    //m_gen = boost::mt19937(9);

    /* Make random list of regions. */
    boost::random::uniform_int_distribution<> rcdist(5,100);

    int region_count = rcdist(m_gen);
    while(region_count-- > 0) m_regions.push_back(m_store.make());

    /* Fill in centre and size biases. */
    boost::random::uniform_real_distribution<> cdist(0.0,1.0);
    boost::random::normal_distribution<> bdist(1.0);

    std::map<double, int> dist_f;
    double last = 0.0;
    int i = 0;
    for(auto region : m_regions) {
        physical_characteristics pc;
        pc.centre = coord_t(cdist(m_gen), cdist(m_gen));
        pc.size_bias = std::max(0.01, bdist(m_gen));

        last += pc.size_bias;
        dist_f[last] = i;
        i ++;
        region->set(pc);
    }
    boost::random::uniform_real_distribution<> expansion_dist(0.0 + 1e-9,
        last - 1e-9);

    auto vregions = generate_voronoi();

    std::vector<std::queue<vregion *>> region_queues(m_regions.size());
    for(unsigned int i = 0; i < vregions.size(); i ++) {
        if(vregions[i].assignment == -1) continue;
        region_queues[vregions[i].assignment].push(&vregions[i]);
        vregions[i].assignment = -1;
    }

    std::vector<std::set<vregion *>> region_sets(m_regions.size());

    unsigned int used = 0;
    while(used != vregions.size()) {
        //std::cout << "Selecting random to expand. Used: " << used << "/" << vregions.size() << std::endl;
        int toexpand = dist_f.upper_bound(expansion_dist(m_gen))->second;
        //std::cout << "\tSelected " << toexpand << " to expand" << std::endl;

        auto &q = region_queues[toexpand];
        //std::cout << "\tQueue length: " << q.size() << std::endl;

        if(q.size()) {
            auto next = q.front(); q.pop();

            if(next->assignment != -1) continue;

            next->assignment = toexpand;
            used ++;

            auto &borders =
                m_regions[toexpand]->get<physical_characteristics>().borders;

            // merge
            std::vector<polygon_t> results;
            boost::geometry::union_(borders, next->outline, results);
            if(results.size() != 1) {
                std::cout << "Failed to merge polygons." << std::endl;
                std::cout << "polygon #1:";
                for(auto &pt : borders.outer()) {
                    std::cout << " (" << pt.x() << "," << pt.y() << ")";
                }
                std::cout << std::endl;
                std::cout << "polygon #2:";
                for(auto &pt : next->outline.outer()) {
                    std::cout << " (" << pt.x() << "," << pt.y() << ")";
                }
                std::cout << std::endl;
                std::cout << results.size() << "!= 1" << std::endl;
                continue;
            }
            borders = results.back();

            region_sets[toexpand].insert(next);

            //std::cout << "\t\tAdding neighbours to queue." << std::endl;
            for(auto n : next->neighbours) {
                if(n->assignment == -1) {
                    q.push(n);
                    //std::cout << "\t\tAdded " << n << std::endl;
                }
            }
        }
    }

    // TODO: check if any regions are inside each other, and if so, change the
    // inner boundaries as appropriate.

    //std::cout << "Regions: " << std::endl;
    for(unsigned int i = 0; i < m_regions.size(); i ++) {
        //std::cout << "\tRegion #" << i << ":" << std::endl;
        //std::cout << "\t\t- Made of " << region_sets[i].size() << " vregions."
            //<< std::endl;
        //double area = 0.0;
        //for(auto v : region_sets[i]) {
            //area += boost::geometry::area(v->outline);
        //}
        //std::cout << "\t\t- Total area: " << area << std::endl;

        polygon_t un;
        std::vector<bool> donev(region_sets[i].size(), false);
        //std::cout << "area: " << boost::geometry::area(m_regions[i]->get<physical_characteristics>().borders) << std::endl;
    }

    // make a pgm output
    std::cout << "P2\n10000 10000\n" << m_regions.size()+1 << std::endl;
    for(int x = 0; x < 10000; x ++) {
        for(int y = 0; y < 10000; y ++) {
            coord_t c(x/10000.0, y/10000.0);

            int colour = m_regions.size();
            for(unsigned int i = 0; i < m_regions.size(); i ++) {
                if(boost::geometry::within(c,
                    m_regions[i]->get<physical_characteristics>().borders)) {

                    colour = i;
                    break;
                }
            }
            std::cout << colour << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<geography::vregion> geography::generate_voronoi() {
    // Notes to future self:
    // - This function is horribly-written.
    // - It's probably not worth trying to fix any problems in this function.
    // - boost::geometry is kind of a horrible geometry library. Why did we
    //      choose it, again?
    // - Rewriting using another Voronoi implementation might be a good idea.
    using pt_t = boost::polygon::point_data<int>;
    using seg_t = boost::polygon::segment_data<int>;
    /* Generate Voronoi diagram. */
    std::vector<pt_t> vpts;
    // TODO: get number of map points from somewhere else.
    const int vpts_count = 100000;

    const int scale = 10000;

    boost::random::uniform_int_distribution<> vdist(0,scale);
    for(int i = 0; i < vpts_count; i ++) {
        vpts.push_back(pt_t(vdist(m_gen), vdist(m_gen)));
    }

    // NOTE: not actually using any segments, but construct_voronoi requires
    // a vector of segments as an argument.
    std::vector<seg_t> vsegs;

    using vd_t = boost::polygon::voronoi_diagram<double>;
    vd_t vd;
    boost::polygon::construct_voronoi(vpts.begin(), vpts.end(),
        vsegs.begin(), vsegs.end(), &vd);

    std::vector<vregion> vregions;
    std::map<const vd_t::cell_type *, int> cell_to_vregion;
    std::set<const vd_t::cell_type *> cells_with_vregions;

    // do two passes over the data.
    for(auto &vc : vd.cells()) {
        auto e = vc.incident_edge();
        bool skip = false;

        do {
            if(e->vertex0() == nullptr || e->vertex1() == nullptr) {
                skip = true;
                break;
            }
            e = e->next();
        } while(e && e != vc.incident_edge());

        if(skip) {
            //std::cout << "skipping " << &vc << std::endl;
            continue;
        }

        vregion vr;
        vr.assignment = -1;
        vregions.push_back(vr);
        //std::cout << "assigning cell_to_vregion[" << &vc << "] to " << vregions.size()-1 << std::endl;
        cell_to_vregion[&vc] = vregions.size()-1;
        cells_with_vregions.insert(&vc);
    }

    for(auto &vc : vd.cells()) {
        if(cells_with_vregions.count(&vc) == 0) continue;
        //std::cout << "&vc: " << &vc << std::endl;
        //std::cout << "count: " << cell_to_vregion.count(&vc) << std::endl;
        int vri = cell_to_vregion[&vc];
        //std::cout << vri << std::endl;
        auto e = vc.incident_edge();
        std::set<std::pair<double, double>> coords;
        do {
            coords.insert(std::make_pair(
                e->vertex0()->x() / scale, e->vertex0()->y() / scale
            ));
            coords.insert(std::make_pair(
                e->vertex1()->x() / scale, e->vertex1()->y() / scale
            ));

            if(cells_with_vregions.count(e->twin()->cell()))
                vregions[vri].neighbours.insert(
                    &vregions[cell_to_vregion[e->twin()->cell()]]);

            e = e->next();
        } while(e && e != vc.incident_edge());

        coord_t c;

        // find centre
        double centre_x = 0.0, centre_y = 0.0;
        for(auto &coord : coords) {
            centre_x += coord.first;
            centre_y += coord.second;
        }
        centre_x /= coords.size();
        centre_y /= coords.size();

        // angle-sort!
        std::vector<std::pair<double, coord_t>> sorted_coords;
        for(auto &coord : coords) {
            sorted_coords.push_back(std::make_pair(std::atan2(
                centre_y - coord.second,
                centre_x - coord.first
            ), coord_t(coord.first, coord.second)));
        }

        // angle-sort.
        std::sort(sorted_coords.begin(), sorted_coords.end(),
            [](const std::pair<double, coord_t> &a,
                const std::pair<double, coord_t> &b) -> bool
                { return a.first < b.first; });

        // make vector of coords
        std::vector<coord_t> ordered_coords;
        for(auto coord : sorted_coords) ordered_coords.push_back(coord.second);
        polygon_t poly;
        boost::geometry::append(poly, ordered_coords);
        vregions[vri].outline = poly;
        boost::geometry::correct(vregions[vri].outline);

        //std::cout << "vregion " << vri << " has " << vregions[vri].neighbours.size() << " neighbours" << std::endl;
    }
    
    // perform mapping of region centres to Voronoi cells
    for(unsigned int i = 0; i < vregions.size(); i ++) {
        int j = 0;
        for(auto const &region : m_regions) {
            if(boost::geometry::within(
                region->get<physical_characteristics>().centre,
                vregions[i].outline)) {
                
                //std::cout << "region " << j << " is within vregion " << i << std::endl;
                vregions[i].assignment = j;
                break;
            }
            j ++;
        }
    }

    return vregions;
}

} // namespace world
