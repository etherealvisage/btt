#ifndef OBJECT_OBJECT_HPP
#define OBJECT_OBJECT_HPP

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <boost/any.hpp>
#include <type_traits>

#include <core/strstream.hpp>

#include "property.hpp"

namespace object {

class store;

class object {
public:
    using id_t = long;
private:
    std::unordered_map<std::type_index, boost::any> m_properties;
    id_t m_id;
    int m_refcount;
    store &m_store;
private:
    friend class store;
    object(store &store, id_t id) : m_id(id), m_refcount(1), m_store(store) {}

    void inc() { m_refcount ++; }
    bool dec() { m_refcount --; return m_refcount == 0; }
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
        return m_properties.insert(
            std::make_pair(std::type_index(typeid(T)), v)).second;
    }
};

} // namespace object

#endif
