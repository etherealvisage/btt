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

class adj_list : public object::property {};
class adj2_list : object::property {};
class wadj_list : public int_prop {
public:
    wadj_list(int v) : int_prop(v) {}
};

void dump_all(const char *why, object::store &st) {
    core::strstream ss;
    st.dump_all(ss);
    std::cout << why << std::endl << ss.operator std::string() << std::endl;
}

int main(int core_unused argc, const char core_unused *argv[]) {
    object::store st;
    object::ref o1 = st.make();
    object::ref o2 = st.make();

    using astruct = object::structure<adj_list>;

    dump_all("Before structure", st);
    {
        astruct as(st);
        as.make_link(o1, o2);

        dump_all("During structure", st);

        as.remove_link(o1, o2);

        dump_all("After removal", st);

        st.gc();

        dump_all("After GC", st);

        as.make_link(o2, o1);

        dump_all("After addition", st);
    }
    dump_all("After structure", st);
    st.gc();
    dump_all("After GC", st);

    return 0;
}
