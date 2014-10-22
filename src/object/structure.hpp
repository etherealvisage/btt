#ifndef OBJECT_STRUCTURE_HPP
#define OBJECT_STRUCTURE_HPP

#include <map>

#include "object.hpp"
#include "property.hpp"
#include "store.hpp"
#include "ref.hpp"

namespace object {

class property;

namespace helpers {

struct per_structure {
    std::map<object::id_t, ref> links;
};

struct structure_property : public property {
public:
    std::map<object::id_t, per_structure> ps;

    virtual void dump(core::strstream &into) {
        bool first = true;
        for(auto p : ps) {
            if(!first) into << ", ";
            else first = false;

            into << p.first << " -> [";
            bool f2 = true;
            for(auto l : p.second.links) {
                if(!f2) into << ", ";
                else f2 = false;
                into << l.first << ": " << l.second->id();
            }
            into << "]";
        }
    }
};

} // namespace helpers

template<typename linkinfo_t>
class structure {
private:
    store &m_store;
    ref m_rootobj;
public:
    structure(store &store) : m_store(store), m_rootobj(store.make()) {}

    void make_link(const ref &from, const ref &to,
        linkinfo_t &&property = linkinfo_t()) {
        
        // if it doesn't have a structure property yet, add one.
        if(!from->has<helpers::structure_property>())
            from->set(helpers::structure_property());
        // if it doesn't have a link object for this structure, add one.
        auto &str = from->get<helpers::structure_property>();
        auto it = str.ps.find(m_rootobj->id());
        if(it == str.ps.end()) {
            str.ps.insert(std::make_pair(m_rootobj->id(),
                helpers::per_structure()));
            it = str.ps.find(m_rootobj->id());
        }

        // create a link object and insert it into the data structures.
        ref link = m_store.make();
        link->set(property);
        // note that the reference is from the root object.
        it->second.links.insert(std::make_pair(to->id(), ref(m_rootobj, link)));
    }

    /*
    void make_link(object::id_t from, object::id_t to,
        linkinfo_t && property = linkinfo_t()) {
        
        // if it doesn't have a structure property yet, add one.
        object &fobj = m_store.get(from);
        if(!fobj.has<structure_property>()) fobj.set(structure_property());
        // if it doesn't have a link object for this structure, add one.
        auto &str = fobj.get<structure_property>();
        auto it = str.find(m_rootobj);
        if(it == str.end()) {
            str.insert(std::make_pair(m_rootobj, std::map<object::id_t, object::id_t>()));
            it = str.find(m_rootobj);
        }

        object &lobject = m_store.make();
        lobject.set(property);

        it->second.insert(std::make_pair(to, lobject.id()));
    }*/
};

} // namespace object

#endif
