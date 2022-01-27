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
#ifndef _object_hpp
#define _object_hpp

#include <boost/optional.hpp>
#include <boost/python.hpp>
#include <map>
#include <optional>
#include <vector>

namespace pyint {
    class parameter {
        private:
            std::string _name;
            boost::optional< boost::python::object > _type;
            boost::optional< boost::python::object > _defaultValue;
        public:
            parameter() {}
            parameter(
                std::string name,
                boost::optional< boost::python::object > type,
                boost::optional< boost::python::object > defaultValue)
                : _name(name), _type(type), _defaultValue(defaultValue) {}

            std::string name() const { return _name; };
            boost::optional< boost::python::object > type() const {
                return _type;
            }
            boost::optional< boost::python::object > defaultValue() const {
                return _defaultValue;
            }
    };

    class signature {
        private:
            boost::optional< boost::python::object > _returnType;
            std::map< std::string, parameter > _parameters;
        public:
            signature() {}
            signature(boost::python::object &target);

            boost::optional< boost::python::object > returnType() const {
                return _returnType;
            }
            std::map< std::string, parameter > &parameters() {
                return _parameters;
            }
    };

    class object : public boost::python::object {
        private:
            void processCallable();
            boost::optional< signature > fnSignature;
        public:
            // Constructs None
            object() : boost::python::object() {}

            template < class T > object(T const &x) : boost::python::object(x) {
                if (!std::is_same< T, boost::python::object >::value) {
                    return;
                }
                if (!callable()) {
                    return;
                }

                processCallable();
            }

            BOOST_PYTHON_DECL explicit object(boost::python::handle<> const &x)
                : boost::python::object(x) {}

            bool callable() const;
            bool commandLineCallable() const;

            std::string call(std::vector< std::string > &stringArguments);
            std::string cli(std::vector< std::string > &rawArguments);
    };
}
#endif