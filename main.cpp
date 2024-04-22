#include <iostream>
#include <vector>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "version.h"
#include "scanner.h"
#include "arg_keywords.h"

namespace options = boost::program_options;

int version() {
    return PROJECT_VERSION_PATCH;
}

using namespace std;

int main(int argc, char **argv)
{
    options::options_description desc("Descriptions");
    desc.add_options()
        ((arg_keywords::PATH + ", p").c_str(), options::value<vector<string>>(), "scanning path") //+
        ((arg_keywords::EXCLUDE + ", e").c_str(), options::value<vector<string>>(), "exluded path") //+
        ((arg_keywords::LEVEL + ", l").c_str(), options::value<uint8_t>(), "scanning level") //+
        ((arg_keywords::SIZE + ", s").c_str(), options::value<uint32_t>(), "minimal file size") //+
        ((arg_keywords::MASK + ", m").c_str(), options::value<string>(), "accepted file name mask") //+
        ((arg_keywords::BLOCK + ", b").c_str(), options::value<uint32_t>(), "compare block size") //+
        ((arg_keywords::FUNCTION + ", f").c_str(), options::value<string>(), "compare function crc or md5")
    ;

    options::variables_map var_map;
    options::store(options::parse_command_line(argc, argv, desc), var_map);
    options::notify(var_map);

    Scanner scanner(var_map);
    scanner.scan();

    return 0;
}
