#ifndef CORE_TYPENAME_HPP
#define CORE_TYPENAME_HPP

#include <string>
#include <typeinfo>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif

namespace core {

#ifdef __GNUG__
inline std::string demangle(const char* name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}
#else
std::string demangle(const char* name) {
    return name;
}
#endif

template<typename T>
inline std::string type_name() {
    return demangle(typeid(T).name());
}

} // namespace core

#endif
