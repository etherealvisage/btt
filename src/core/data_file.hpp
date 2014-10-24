#ifndef CORE_DATA_FILE_HPP
#define CORE_DATA_FILE_HPP

#include <string>
#include <istream>
#include <fstream>

#ifndef DATA_PATH
    #define DATA_PATH "data/"
#endif

namespace core {

class data_file {
private:
    std::string m_path;
    std::ifstream m_last_stream;
public:
    data_file(const std::string &subpath) { m_path = DATA_PATH + subpath; }

    std::istream &stream() {
        m_last_stream.open(m_path.c_str());

        return m_last_stream;
    }
};

} // namespace core

#endif
