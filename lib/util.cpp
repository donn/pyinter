// Copyright 2022 Mohamed Gaber
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "util.hpp"

#include <cstdlib>
#include <sstream>
#include <stdexcept>

boost::optional< std::string > util::env(std::string key) {
    auto value = getenv(key.c_str());
    if (value == nullptr) {
        return boost::none;
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