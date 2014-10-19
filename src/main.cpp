#include <iostream>
#include <cstdio>

#include <core/strstream.hpp>
#include <core/unused.hpp>

#include <object/object.hpp>
#include <object/filter.hpp>
#include <object/structure.hpp>
#include <object/store.hpp>

#include <iostream>
#include <string>

class int_prop : object::property {
public:
    int_prop(int v = 0) : val(v) {}
    int val;
};

class str_prop : object::property {
public:
    std::string val;
};

class adj_list : object::property {};
class adj2_list : object::property {};
class wadj_list : public int_prop {
public:
    wadj_list(int v) : int_prop(v) {}
};

int main(int core_unused argc, const char core_unused *argv[]) {
    object::store st;
    object::object obj1 = st.make(), obj2 = st.make();

    obj1.set(int_prop());
    obj2.set(str_prop());

    if(object::filter<int_prop>::matches(obj1)) std::cout << "obj1 has int" << std::endl;
    if(object::filter<int_prop>::matches(obj2)) std::cout << "obj2 has int" << std::endl;

    obj1.set(str_prop());

    if(object::filter<int_prop,str_prop>::matches(obj1)) std::cout << "obj1 has int/str" << std::endl;
    if(object::filter<int_prop,str_prop>::matches(obj2)) std::cout << "obj2 has int/str" << std::endl;

    using astruct = object::structure<adj_list>;
    astruct::make_link(obj1, obj2);

    if(astruct::is_linked(obj1, obj2)) std::cout << "obj1 -> obj2 linked" << std::endl;
    if(astruct::is_linked(obj2, obj1)) std::cout << "obj2 -> obj1 linked" << std::endl;

    using a2struct = object::structure<adj_list>;
    astruct::make_link(obj2, obj2);
    if(a2struct::is_linked(obj2, obj2)) std::cout << "obj2 -> obj2 2linked" << std::endl;

    using wstruct = object::structure<wadj_list>;
    wstruct::make_link(obj1, obj2, std::move(wadj_list(42)));

    std::cout << "obj1 -> obj2 link weight: " << wstruct::get_link(obj1, obj2).val << std::endl;

    auto astruct2 = object::structure2<adj_list>();

    astruct2.add_link(obj1, obj2);

    auto astruct3 = object::structure3<adj_list>(st);

    astruct3.make_link(obj1.id(), obj2.id());

    return 0;
}
