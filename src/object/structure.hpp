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

class link_prop : public property {
private:
    ref m_ref;
public:
    link_prop(ref ref) : m_ref(ref) {}

    const ref &target() const { return m_ref; }
};

class link_source : public link_prop {
public:
    link_source(ref ref) : link_prop(ref) {}
};
class link_target : public link_prop {
public:
    link_target(ref ref) : link_prop(ref) {}
};

struct structure_property : public property {
public:
    std::map<object::id_t, std::map<object::id_t, ref>> links;

    virtual void dump(std::ostream &into) {
        bool first = true;
        for(auto p : links) {
            if(!first) into << ", ";
            else first = false;

            into << p.first << " -> [";
            bool f2 = true;
            for(auto l : p.second) {
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
    structure(store &store) : m_store(store), m_rootobj(store.make()) {
        m_rootobj->set(helpers::structure_property());
    }

    void make_link(const ref &from, const ref &to,
        linkinfo_t &&property = linkinfo_t()) {
        
        // if it doesn't have a link object for this structure, add one.
        auto &str = m_rootobj->get<helpers::structure_property>();
        auto it = str.links.find(from->id());
        if(it == str.links.end()) {
            str.links.insert(std::make_pair(from->id(),
                std::map<object::id_t, ref>()));
            it = str.links.find(from->id());
        }

        // create a link object and insert it into the data structures.
        ref link = m_store.make();
        link->set(property);
        link->set(helpers::link_source(from));
        link->set(helpers::link_target(to));
        // note that the reference is from the root object.
        if(!(it->second.insert(std::make_pair(to->id(),
            ref(m_rootobj, std::move(link))))).second)
                throw std::runtime_error("Cannot add already present link");
    }

    bool has_link(const ref &from, const ref &to) {
        return link_raw(from, to) != nullptr;
    }

    linkinfo_t &link(const ref &from, const ref &to) {
        linkinfo_t *raw = link_raw(from, to);
        if(raw == nullptr) throw std::runtime_error("Need to have linkinfo_t!");
        return *raw;
    }

    void remove_link(const ref &from, const ref &to) {
        auto &str = m_rootobj->get<helpers::structure_property>();
        auto it = str.links.find(from->id());
        if(it == str.links.end())
            throw std::runtime_error("Cannot remove nonexistent link");
        auto it2 = it->second.find(to->id());
        if(it2 == it->second.end()) 
            throw std::runtime_error("Cannot remove nonexistent link");
        it->second.erase(it2);

        if(it->second.size() == 0) str.links.erase(it);
    }
private:
    linkinfo_t *link_raw(const ref &from, const ref &to) {
        auto &str = m_rootobj->get<helpers::structure_property>();
        auto it = str.links.find(from->id());
        if(it == str.links.end()) return nullptr;
        auto it2 = it->second.find(to->id());
        if(it2 == it->second.end()) return nullptr;
        return &it2->second->get<linkinfo_t>();
    }
};

} // namespace object

#endif
