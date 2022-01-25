#include "util.hpp"

#include <boost/format.hpp>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

std::string util::env(std::string key) {
    auto value = getenv(key.c_str());
    if (value == nullptr) {
        std::string errorMessage =
            (boost::format("Environment variable %s not found.") % key).str();
        throw std::runtime_error(errorMessage.c_str());
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