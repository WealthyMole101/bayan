#include "scanner.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/fstream.hpp>

#include "arg_keywords.h"

using namespace std;
using namespace boost::filesystem;

Scanner::Scanner(boost::program_options::variables_map& var_map)
{
    m_path.push_back(".");
    m_exclude_path.push_back("");
    m_level = 2;
    m_min_file_size = 10;
    m_block_size = 1024;
    m_file_name_mask = std::shared_ptr<boost::regex>(new boost::regex("[a-zA-Z0-9_\/.]*"));
    m_is_md5 = "md5";

    m_path = !var_map[arg_keywords::PATH].empty() ? var_map[arg_keywords::PATH].as<vector<string>>() : m_path;
    m_exclude_path = !var_map[arg_keywords::EXCLUDE].empty() ? var_map[arg_keywords::EXCLUDE].as<vector<string>>() : m_exclude_path;
    m_level = !var_map[arg_keywords::LEVEL].empty() ? var_map[arg_keywords::LEVEL].as<uint8_t>() - 0x30:  m_level;
    m_min_file_size = !var_map[arg_keywords::SIZE].empty() ? var_map[arg_keywords::SIZE].as<uint32_t>() : m_min_file_size;
    m_block_size = !var_map[arg_keywords::BLOCK].empty() ? var_map[arg_keywords::BLOCK].as<uint32_t>() : m_block_size;
    m_file_name_mask = !var_map[arg_keywords::MASK].empty() ? std::shared_ptr<boost::regex>(new boost::regex(var_map[arg_keywords::MASK].as<string>())) : m_file_name_mask;
    m_is_md5 = !var_map[arg_keywords::FUNCTION].empty() ? var_map[arg_keywords::FUNCTION].as<string>() == "md5" : m_is_md5;
}

void Scanner::scan()
{
    Comparator comparator(m_block_size, m_level, m_is_md5);
    list<list<string>> result;

    for (auto& path: m_path) {
        for (recursive_directory_iterator it(path, directory_options::skip_permission_denied | directory_options::skip_dangling_symlinks), end;
             it != end; ++it) {
            if (it.level() == m_level) {
                it.disable_recursion_pending(true);
            }

            if (find_if(m_exclude_path.begin(), m_exclude_path.end(),
                        [&](string& x){return it->path().string().compare(x) >= 0;}) != m_exclude_path.end()) {
                continue;
            }

            boost::cmatch match_result;

            if (is_regular_file(it->path()) &&
                boost::regex_match(it->path().c_str(), match_result, *m_file_name_mask.get())) {
                if (file_size(it->path()) > m_min_file_size) {                    
//                    cout << it->path().string() << " " <<file_size(it->path()) << " " << m_min_file_size << endl;
                    list<string> res = comparator.Compare(it->path().string(), file_size(it->path()), path);

                    if (!res.empty()) {
                        res.sort();
                        auto r = find_if(result.begin(), result.end(),
                                      [&](list<string>& x){ return equal(x.begin(), x.end(), res.begin(), res.end(),
                                                                        [&](string& a, string& b){return a.compare(b) == 0;});});
                        if (r == result.end()) {
                            result.push_back(res);
                        }
                    }
                }
            }
        }
    }

    for (auto item: result) {
        for (auto file: item) {
            cout << file << endl;
        }
        cout << endl;
    }
}
