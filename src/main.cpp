#include <iostream>
#include <cstdio>
#include <string>
#include <cctype>

#include <core/strstream.hpp>
#include <core/unused.hpp>
#include <core/data_file.hpp>

#include <object/object.hpp>
#include <object/filter.hpp>
#include <object/structure.hpp>
#include <object/store.hpp>

#include <world/geography.hpp>
#include <world/name_generator.hpp>

class int_prop : object::property {
public:
    int_prop(int v = 0) : val(v) {}
    int val;
};

class str_prop : object::property {
public:
    std::string val;
};

class adj_list : public object::property {};
class adj2_list : object::property {};
class wadj_list : public int_prop {
public:
    wadj_list(int v) : int_prop(v) {}
};

void dump_all(const char *why, object::store &st) {
    std::cout << why << std::endl;
    st.dump_all(std::cout);
}

int main(int core_unused argc, const char core_unused *argv[]) {
    object::store st;
    
    world::geography geog(st);

    geog.generate();

    world::name_generator ng;
    ng.load_trigraphs("male", core::data_file("/names-male.tri").stream());
    ng.load_trigraphs("female", core::data_file("/names-female.tri").stream());

    boost::random::mt19937 gen;
    for(int i = 0; i < 100; i ++) {
        std::string name = ng.generate_name("female", gen);
        name[0] = std::toupper(name[0]);
        std::cout << name << std::endl;
    }
    for(int i = 0; i < 100; i ++) {
        std::string name = ng.generate_name("male", gen);
        name[0] = std::toupper(name[0]);
        std::cout << name << std::endl;
    }

    return 0;
}
