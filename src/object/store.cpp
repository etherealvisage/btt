#include <iostream>
#include <queue>

#include "store.hpp"
#include "ref.hpp"

namespace object {

ref store::make() {
    object::id_t id = m_lastid ++;

    return ref(m_objects.insert(std::make_pair(id,
        object(*this, id))).first->second);
}

void store::gc() {
    std::queue<object::id_t> tovisit;
    tovisit.push(object::invalid_id);

    std::set<object::id_t> seen;
    seen.insert(object::invalid_id);

    while(tovisit.size() > 0) {
        object::id_t next = tovisit.front(); tovisit.pop();
        
        for(auto it : m_refs[next]) {
            if(seen.count(it.first)) continue;
            seen.insert(it.first);
            tovisit.push(it.first);
        }
    }

    std::vector<object::id_t> todestroy;
    for(auto it : m_objects) {
        //std::cout << "object " << it.first << " count: " << seen.count(it.first) << std::endl;
        if(seen.count(it.first)) continue;
        else {
            todestroy.push_back(it.first);
        }
    }
    for(auto id : todestroy) destroy(id);
}

void store::dump_all(core::strstream &into) {
    for(auto &object : m_objects) {
        object.second.dump(into);
        into << "\n";
    }
}

} // namespace object
