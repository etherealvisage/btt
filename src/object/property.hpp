#ifndef OBJECT_PROPERTY_HPP
#define OBJECT_PROPERTY_HPP

#include <core/strstream.hpp>

namespace object {

class property {
public:
    virtual ~property() = default;

    virtual void dump(std::ostream &) {}
};

} // namespace object

#endif
