#include <iostream>

#include <boost/random/uniform_real_distribution.hpp>

#include "name_generator.hpp"

namespace world {

void name_generator::load_trigraphs(const std::string &identifier,
    std::istream &stream) {

    trigraphs_t result;

    char a, b, c;
    double p;
    std::pair<char, char> last;
    double cumulative = 0.0;
    while(!stream.eof() && stream.peek() && !stream.eof()) {
        a = stream.get();
        stream.get();
        b = stream.get();
        stream.get();
        c = stream.get();
        stream >> p;
        stream.get(); // newline

        auto pair = std::make_pair(a,b);
        if(pair != last) {
            cumulative = p;
            last = pair;
        }
        else cumulative += p;
        result[std::make_pair(a,b)][cumulative] = c;
    }

    m_trigraphs[identifier] = result;
}

std::string name_generator::generate_name(const std::string &dataset,
    boost::mt19937 &gen) {

    std::string result = "XX";
    boost::random::uniform_real_distribution<> cdist(0.0 + 1e-9, 1.0 - 1e-9);
    auto &trigraphs = m_trigraphs[dataset];
    do {
        double val = cdist(gen);
        auto prev = std::make_pair(result[result.length()-2],
            result[result.length()-1]);
        result += trigraphs[prev].upper_bound(val)->second;
    } while(result.back() != 'X' && result.size()<10);

    result.pop_back();

    return result.substr(2);
}

} // namespace world
