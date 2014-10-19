#ifndef OBJECT_STORE_HPP
#define OBJECT_STORE_HPP

#include <map>

#include "object.hpp"

namespace object {

class store {
private:
    std::map<object::id_t, object> m_objects;
    object::id_t m_lastid;
protected:
    friend class object;
    void destroy(object::id_t id) {
        if(m_objects.count(id) == 0)
            throw std::runtime_error("Object does not exist in store");
        m_objects.erase(m_objects.find(id));
    }
public:
    store() : m_lastid(1) {}

    object &make() {
        object::id_t id = m_lastid ++;
        return m_objects.insert(
            std::make_pair(id, object(*this, id))).first->second;
    }

    object &get(object::id_t id) {
        if(m_objects.count(id) == 0)
            throw std::runtime_error("Object does not exist in store");
        return m_objects.find(id)->second;
    }

    const object &get(object::id_t id) const {
        if(m_objects.count(id) == 0)
            throw std::runtime_error("Object does not exist in store");
        return m_objects.find(id)->second;
    }

    void inc(object::id_t id) {
        get(id).inc();
    }

    void dec(object::id_t id) {
        if(get(id).dec()) destroy(id);
    }
};

} // namespace object

#endif
