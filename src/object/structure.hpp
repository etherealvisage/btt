#ifndef OBJECT_STRUCTURE_HPP
#define OBJECT_STRUCTURE_HPP

#include <map>

#include "object.hpp"
#include "property.hpp"
#include "store.hpp"

namespace object {

class property;

template<typename link_type>
class structure2 {
private:
    std::map<object::id_t, std::map<object::id_t, link_type>> m_links;
public:
    void add_link(const object &from, const object &to,
        link_type &&link = link_type()) {

        add_link(from.id(), to.id(), std::move(link));
    }

    void add_link(object::id_t from, object::id_t to,
        link_type &&link = link_type()) {
        m_links[from][to] = link;
    }
    
    bool has_link(const object &from, const object &to) const
        { return has_link(from.id(), to.id()); }

    bool has_link(object::id_t from, object::id_t to) const {
        if(m_links.count(from) == 0) return false;
        const auto &imap = m_links.find(from)->second;
        return imap.find(to) != imap.end();
    }

    link_type &link(object &from, object &to) {
        if(!has_link(from.id(), to.id()))
            throw std::runtime_error("No structural link between requested objects");

        return m_links[from][to];
    }

    const link_type &link(object &from, object &to) const {
        if(!has_link(from.id(), to.id()))
            throw std::runtime_error("No structural link between requested objects");
        
        return m_links.find(from)->second.find(to)->second;
    }

    void remove_link(object &from, object &to) {
        if(!has_link(from.id(), to.id()))
            throw std::runtime_error("Cannot remove nonexistent edge.");
    }
};

template<typename fundamental>
class structure {
private:
    class link_t {
        object &obj;
        fundamental property;
        link_t(object &obj, fundamental && property) : obj(obj), property(std::move(property)) {}
        friend class structure<fundamental>;
    };
    class adj_t : property {
        std::map<object::id_t, link_t> m_links;
        friend class structure<fundamental>;
    };
public:
    static bool make_link(object &from, object &target, fundamental && property = fundamental()) {
        if(!from.has<adj_t>()) from.set(adj_t());
        auto &links = from.get<adj_t>().m_links;
        // already present?
        auto it = links.find(target.id());
        if(it != links.end()) return false;

        links.insert(std::make_pair(target.id(), link_t(target, std::move(property))));

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

        return it->second.property;
    }
};

// object.structure_property[structure ID][destination ID] = edge object ID
class structure_property : property,
    public std::map<object::id_t, std::map<object::id_t, object::id_t>> {};

template<typename linkinfo_t>
class structure3 {
private:
    class link_t : public property {
    public:
        
    };
private:
    store &m_store;
    object::id_t m_rootobj;
public:
    structure3(store &store) : m_store(store) {
        m_rootobj = store.make().id();
    }

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
    }

    linkinfo_t &get_link(object::id_t from, object::id_t to) {
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
    }
};

} // namespace object

#endif
