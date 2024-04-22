#include "comparator.h"

#include <iostream>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost::uuids::detail;

Comparator::Comparator(uint32_t block_size, uint32_t level, bool is_md5): m_block_size(block_size), m_level(level), m_is_md5(is_md5)
{
    m_block0 = make_shared<char*>(new char[m_block_size]);
    m_block1 = make_shared<char*>(new char[m_block_size]);
}

list<string> Comparator::Compare(std::string reference_file, uint32_t reference_file_size, std::string path)
{
    list<string> result;

    for (recursive_directory_iterator it(path, directory_options::skip_permission_denied | directory_options::skip_dangling_symlinks), end; it != end; ++it) {
        if (it.level() == m_level) {
            it.disable_recursion_pending(true);
        }

        if (it->path().string() == reference_file) {
            continue;
        }

        auto current_file = it->path();

        if (is_regular_file(current_file)) {
            auto current_file_size = file_size(it->path());
            uint32_t tail = reference_file_size % m_block_size;

            boost::filesystem::ifstream file0;
            boost::filesystem::ifstream file1;
            file0.open(reference_file, boost::filesystem::ifstream::in);
            file1.open(current_file, boost::filesystem::ifstream::in);

            if (file0.fail() || file1.fail()) {
                continue;
            }

            bool hash_equal = false;

            while (file0.peek() != EOF || file1.peek() != EOF) {
                hash_equal = false;

                file0.read(*m_block0.get(), m_block_size);
                file1.read(*m_block1.get(), m_block_size);

                if (file0.gcount() != file1.gcount()) {
                    break;
                }

                if (file0.gcount() < m_block_size) {
                    memset(*m_block0.get() + file0.gcount(), 0, m_block_size - file0.gcount());
                }

                if (file1.gcount() < m_block_size) {
                    memset(*m_block1.get() + file1.gcount(), 0, m_block_size - file1.gcount());
                }

                if (!calculate()) {
                    break;
                }

                hash_equal = true;
            }

            //конец одного из файлов
            //Если не конец у одного из файлов то они не равны, переходим к следующему.
            if (!hash_equal || file0.peek() != EOF || file1.peek() != EOF) {
                file0.close();
                file1.close();
                continue;
            }

            file0.close();
            file1.close();

            //Если конец у обоих файлов то они равны.
            result.push_back(it->path().string());
        }
    }

    if (!result.empty()) {
        result.insert(result.begin(), reference_file);
    }

    return std::move(result);
}

bool Comparator::calculate()
{
    bool result = true;
    if (m_is_md5) {
        auto md5_object0 = md5();
        auto md5_object1 = md5();
        md5_object0.process_bytes(*m_block0.get(), m_block_size);
        md5_object1.process_bytes(*m_block1.get(), m_block_size);

        md5::digest_type hash0;
        md5::digest_type hash1;
        md5_object0.get_digest(hash0);
        md5_object1.get_digest(hash1);

        if (hash0[0] != hash1[0] ||
            hash0[1] != hash1[1] ||
            hash0[2] != hash1[2] ||
            hash0[3] != hash1[3]) {
            result = false;
        }
    } else {
        boost::crc_32_type result0;
        boost::crc_32_type result1;
        result0.process_bytes(*m_block0.get(), m_block_size);
        result1.process_bytes(*m_block1.get(), m_block_size);

        if (result0.checksum() != result1.checksum()) {
            result = false;
        }
    }

    return result;
}
