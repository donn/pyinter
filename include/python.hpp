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
#ifndef _python_hpp
#define _python_hpp

#include <boost/python.hpp>

// Various assisting functions for boost::python
namespace boost {
    namespace python {
        inline void traceback() { PyErr_Print(); }

        inline const char *describe(boost::python::object &obj) {
            return boost::python::extract< const char * >(str(obj));
        }

        inline const char *pyname(boost::python::object &obj) {
            boost::python::object nameAttribute = obj.attr("__name__");
            return boost::python::extract< const char * >(nameAttribute);
        }

        inline bool hasattr(boost::python::object &obj, std::string name) {
            return PyObject_HasAttrString(obj.ptr(), name.c_str());
        }
    }
}

#endif