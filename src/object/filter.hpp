#ifndef OBJECT_FILTER_HPP
#define OBJECT_FILTER_HPP

namespace object {

class object;

template<typename ...Required>
class filter;

template<typename T>
class filter<T> {
public:
    static bool matches(const object &obj) {
        return obj.has<T>();
    }
};

template<typename T, typename ...Required>
class filter<T, Required...> {
public:
    static bool matches(const object &obj) {
        return obj.has<T>() && filter<Required...>::matches(obj);
    }
};

} // namespace object

#endif
