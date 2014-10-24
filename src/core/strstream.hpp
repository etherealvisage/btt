#ifndef CORE_STRSTREAM_HPP
#define CORE_STRSTREAM_HPP

#include <string>
#include <sstream>

namespace core {

class strstream {
private:
    std::ostringstream m_ss;
public:
    template<typename T>
    strstream &operator<<(const T &v) {
        m_ss << v;
        return *this;
    }

    operator std::string() const { return m_ss.str(); }
};

} // namespace core

#endif
