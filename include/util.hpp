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
#ifndef _util_hpp
#define _util_hpp

#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace util {
    boost::optional< std::string > env(std::string key);
    std::vector< std::string > split(const std::string *string, char delimiter);
    std::string join(std::vector< std::string > *array, char delimiter);
};

#endif