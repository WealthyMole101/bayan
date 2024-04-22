#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/program_options/variables_map.hpp>

#include "comparator.h"

class Scanner
{
public:
//    explicit Scanner(const std::vector<std::string>& path, const std::vector<std::string>& exclude_path = std::vector<std::string>(),
//                     uint8_t level = 0, uint32_t min_file_size = 1, const std::string& file_name_mask ="");
    explicit Scanner(boost::program_options::variables_map& var_map);

    void scan();

private:
    std::vector<std::string> m_path;
    std::vector<std::string> m_exclude_path;
    uint8_t m_level;
    uint32_t m_min_file_size;
    uint32_t m_block_size;
    std::shared_ptr<boost::regex> m_file_name_mask;
    bool m_is_md5;
};

#endif // SCANNER_H
