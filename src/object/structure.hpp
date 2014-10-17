#ifndef OBJECT_STRUCTURE_HPP
#define OBJECT_STRUCTURE_HPP

#include <map>

#include "object.hpp"
#include "property.hpp"

namespace object {

class property;

template<typename fundamental>
class structure {
public:
    class link_t : property {
    private:
        object &m_target;
        fundamental m_wrapped;
    private:
        link_t(object &target, fundamental && towrap) : m_target(target), m_wrapped(towrap) {}

        friend class structure<fundamental>;
    };
    class adj_t : property {
    private:
        std::map<object::id_t, object> m_links;
        friend class structure<fundamental>;
    };
private:
    
public:
    static bool make_link(object &from, object &target, fundamental && property = fundamental()) {
        object link;
        if(!from.has<adj_t>()) from.set(adj_t());
        auto &links = from.get<adj_t>().m_links;
        // already present?
        auto it = links.find(target.id());
        if(it != links.end()) return false;

        link.set(link_t(target, std::move(property)));
        
        links.insert(std::make_pair(target.id(), std::move(link)));

        return true;
    }

    static bool is_linked(object &from, object &target) {
        if(!from.has<adj_t>()) return false;
        return from.get<adj_t>().m_links.count(target.id()) == 1;
    }

    static fundamental &get_link(object &from, object &target) {
        auto &links = from.get<adj_t>().m_links;
        auto it = links.find(target.id());
        if(it == links.end()) throw std::runtime_error("no such link");

        return (it->second).template get<link_t>().m_wrapped;
    }
};

} // namespace object

#endif
