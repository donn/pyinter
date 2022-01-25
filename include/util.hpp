#ifndef _util_hpp
#define _util_hpp

#include <string>
#include <vector>

namespace util {
    std::string env(std::string key);
    std::vector< std::string > split(const std::string *string, char delimiter);
    std::string join(std::vector< std::string > *array, char delimiter);
};

#endif