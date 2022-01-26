#include "util.hpp"

#include <cstdlib>
#include <sstream>
#include <stdexcept>

std::optional< std::string > util::env(std::string key) {
    auto value = getenv(key.c_str());
    if (value == nullptr) {
        return std::nullopt;
    }
    return std::string(value);
}

std::vector< std::string >
util::split(const std::string *string, char delimiter) {
    std::vector< std::string > returnValue;
    std::stringstream ss;
    ss.str(*string);

    std::string item;
    while (std::getline(ss, item, delimiter)) {
        returnValue.push_back(item);
    }

    return returnValue;
}

std::string join(std::vector< std::string > *array, char delimiter) {
    std::stringstream ss;

    for (auto &string : *array) {
        ss << string << delimiter;
    }

    return ss.str();
}