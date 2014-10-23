#ifndef WORLD_NAME_GENERATOR_HPP
#define WORLD_NAME_GENERATOR_HPP

#include <string>
#include <istream>
#include <map>

#include <boost/random/mersenne_twister.hpp>

namespace world {

class name_generator {
private:
    using trigraphs_t = std::map<std::pair<char, char>,
        std::map<double, char>>;
    std::map<std::string, trigraphs_t> m_trigraphs;
public:
    name_generator() {}

    void load_trigraphs(const std::string &identifier, std::istream &stream);

    std::string generate_name(const std::string &dataset, boost::mt19937 &gen);
};

} // namespace world

#endif
