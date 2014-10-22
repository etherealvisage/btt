#ifndef OBJECT_STORE_HPP
#define OBJECT_STORE_HPP

#include <map>
#include <set>

#include "object.hpp"

namespace object {

class ref;

class store {
private:
    std::map<object::id_t, object> m_objects;
    //std::map<std::pair<object::id_t, object::id_t>, int> m_refs;
    std::map<object::id_t, std::map<object::id_t, int>> m_refs;
    object::id_t m_lastid;
protected:
    // keep these functions hidden to prevent them from being misused.
    friend class ref;
    void add_ref(object::id_t context, object::id_t target) {
        //std::cout << "adding reference between " << context << " and " << target << " (currently " << m_refs[context][target] << ")" << std::endl;
        m_refs[context][target] ++;
    }
    void remove_ref(object::id_t context, object::id_t target) {
        //std::cout << "removing reference between " << context << " and " << target  << " (currently " << m_refs[context][target] << ")" << std::endl;
        m_refs[context][target] --;
        if(m_refs[context][target] == 0) m_refs[context].erase(target);
        if(m_refs[context].size() == 0) m_refs.erase(context);
    }

    object &get(object::id_t id) {
        if(m_objects.count(id) == 0)
            throw std::runtime_error("Object does not exist in store");
        return m_objects.find(id)->second;
    }

    void destroy(object::id_t id) {
        if(m_objects.count(id) == 0)
            throw std::runtime_error("Object does not exist in store");
        m_objects.erase(m_objects.find(id));
    }
public:
    store() : m_lastid(1) {}

    ref make();

    void gc();

    void dump_all(std::ostream &into);
};

} // namespace object

#endif
