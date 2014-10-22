#ifndef OBJECT_OBJECT_HPP
#define OBJECT_OBJECT_HPP

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <boost/any.hpp>
#include <type_traits>

#include <core/strstream.hpp>
#include <core/typename.hpp>

#include "property.hpp"

namespace object {

class store;
class ref;

class object {
public:
    using id_t = long;
    static constexpr id_t invalid_id = -1;
private:
    std::unordered_map<std::type_index, boost::any> m_properties;
    std::unordered_map<std::type_index, std::function<void (core::strstream &, boost::any &)>> m_dump_helpers;
    //std::unordered_map<std::type_index, void (core::strstream &, boost::any &)> m_dump_helpers;
    id_t m_id;
    friend class ref;
    store &m_store;
private:
    friend class store;
    object(store &store, id_t id) : m_id(id), m_store(store) {}
public:
    id_t id() const { return m_id; }

    template<typename T>
    bool has() const {
        static_assert(std::is_base_of<property, T>::value,
            "Must request property");
        return m_properties.count(std::type_index(typeid(T))) == 1;
    }

    template<typename T>
    T *get_raw() {
        static_assert(std::is_base_of<property, T>::value,
            "Must request property");
        auto it = m_properties.find(std::type_index(typeid(T)));
        if(it == m_properties.end())
            throw std::runtime_error(
                core::strstream() << "Tried to get non-contained property");
        return &boost::any_cast<T &>(it->second);
    }

    template<typename T>
    T &get() {
        return *get_raw<T>();
    }

    template<typename T>
    const T &get() const {
        return *get_raw<T>();
    }


    template<typename T>
    bool set(T && v) {
        std::function<void (core::strstream &, boost::any &)> wrapper_fct(dump_wrapper<T>);
        m_dump_helpers[std::type_index(typeid(T))] = wrapper_fct;
        return m_properties.insert(
            std::make_pair(std::type_index(typeid(T)), v)).second;
    }

    void dump(core::strstream &into) {
        into << "[Object " << m_id << "] = {";
        bool first = true;
        for(auto &prop : m_properties) {
            if(!first) into << ", ";
            else first = false;
            into << core::demangle(prop.second.type().name()) << " = (";
            m_dump_helpers[prop.second.type()](into, prop.second);
            into << ")";
        }
        into << "}";
    }
private:
    template<typename T>
    static void dump_wrapper(core::strstream &s, boost::any &p) {
        boost::any_cast<T>(p).dump(s);
    }
};

} // namespace object

#endif
