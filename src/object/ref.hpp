#ifndef OBJECT_REF_HPP
#define OBJECT_REF_HPP

#include "object.hpp"
#include "store.hpp"

namespace object {

class store;

class ref {
private:
    store &m_store;
    object::id_t m_context;
    object &m_target;
public:
    /** Constructs an outside reference to the given object. */
    ref(const object &object) : m_store(object.m_store),
        m_context(object::invalid_id), m_target(m_store.get(object.id()))
        { cons(); }
    /** Constructs an inside reference to the target from the given context. */
    ref(const object &context, const object &target) :
        m_store(context.m_store), m_context(context.id()),
        m_target(m_store.get(target.id()))
        { check_same(context, target); cons(); }
    ref(const ref &context, const ref &target) : ref(*context, *target) {}
    ref(const object &context, const ref &target) : ref(context, *target) {}
    ref(const ref &context, const object &target) : ref(*context, target) {}
    /** Copy constructor. */
    ref(const ref &other) : m_store(other.m_store), m_target(other.m_target) {
        m_context = other.m_context;
        cons();
    }
    ~ref() { dcon(); }

    object::id_t context() const { return m_context; }
    object::id_t target() const { return m_target.id(); }

    object *operator->() const { return &m_target; }
    object &operator*() { return m_target; }
    const object &operator*() const { return m_target; }
private:
    inline void check_same(const object &a, const object &b) {
        if(&a.m_store != &b.m_store) 
            throw std::runtime_error("Can only have reference context from "
                "same object store as target");
    }
    inline void cons() { m_store.add_ref(m_context, m_target.id()); }
    inline void dcon() { m_store.remove_ref(m_context, m_target.id()); }
};

} // namespace object

#endif
