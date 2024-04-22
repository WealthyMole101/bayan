#ifndef COMPARATOR_H
#define COMPARATOR_H

#include <cstdint>
#include <list>
#include <boost/filesystem.hpp>

class Comparator
{
public:
    explicit Comparator(uint32_t block_size, uint32_t level, bool is_md5);
    std::list<std::string> Compare(std::string reference_file, uint32_t reference_file_size, std::string path);

private:
    bool calculate();
    uint32_t m_block_size;
    uint32_t m_level;
    std::shared_ptr<char*> m_block0;
    std::shared_ptr<char*> m_block1;
    bool m_is_md5;
};

#endif // COMPARATOR_H
