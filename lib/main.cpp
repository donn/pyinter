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
#include "object.hpp"
#include "python.hpp"
#include "util.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tcl.h>
#include <unordered_map>
#include <vector>

std::vector< std::shared_ptr< pyint::object > > objectStore;

extern "C" int RunPythonCommand(
    ClientData cdata,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const objv[]) {
    using namespace boost::python;

    try {
        auto fn = *static_cast< pyint::object * >(cdata);

        if (!fn.callable()) {
            auto returnValue = extract< const char * >(str(fn));
            Tcl_SetObjResult(interp, Tcl_NewStringObj(returnValue, -1));
            return TCL_OK;
        }

        std::vector< std::string > stringArguments;
        for (int i = 1; i < objc; i += 1) {
            auto tclArg = objv[i];
            auto name = const_cast< const char * >(tclArg->bytes);
            stringArguments.push_back(name);
        }

        std::string returnValue = fn.commandLineCallable() ?
                                      fn.cli(stringArguments) :
                                      fn.call(stringArguments);

        Tcl_SetObjResult(interp, Tcl_NewStringObj(returnValue.c_str(), -1));

        return TCL_OK;

    } catch (boost::python::error_already_set &e) {
        handle_exception();
        traceback();
        return TCL_ERROR;
    }
}

extern "C" int Pyinter_Import(
    ClientData cdata,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const objv[]) {
    using namespace boost::python;

    try {
        if (objc != 2) {
            std::cerr << "Syntax: " << objv[0]->bytes
                      << " <name of python module>" << std::endl;
            return TCL_ERROR;
        }

        auto searchPathsOptional = util::env("PYINTER_SEARCH_PATHS");
        auto searchPathsUnwrapped =
            searchPathsOptional.value_or(util::env("PWD").value());
        auto searchPaths = util::split(&searchPathsUnwrapped, ':');

        list searchPathsPy = extract< list >(import("sys").attr("path"));
        for (auto &path : searchPaths) {
            str pathPy = str(path.c_str());
            searchPathsPy.append(pathPy);
        }

        auto importName = std::string(objv[1]->bytes);

        auto ns = Tcl_CreateNamespace(interp, importName.c_str(), NULL, NULL);
        Tcl_Export(interp, ns, "*", 0);

        object importable = import(importName.c_str());
        object getmembers = import("inspect").attr("getmembers");
        list members = extract< list >(getmembers(importable));

        for (ssize_t i = 0; i < len(members); i += 1) {
            tuple member = extract< tuple >(members[i]);
            const char *name = extract< const char * >(member[0]);
            std::string namespaced = importName + "::" + name;

            object objectRef = extract< object >(member[1]);
            auto pointer = std::make_shared< pyint::object >(objectRef);

            objectStore.push_back(pointer);

            Tcl_CreateObjCommand(
                interp,
                namespaced.c_str(),
                RunPythonCommand,
                pointer.get(),
                NULL);
        }

        return TCL_OK;

    } catch (boost::python::error_already_set &e) {
        handle_exception();
        traceback();
        return TCL_ERROR;
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return TCL_ERROR;
    }
}

// Called when Tcl loads the extension
extern "C" int DLLEXPORT Pyinter_Init(Tcl_Interp *interp) {
    using namespace boost::python;
    try {
        Py_Initialize();
        if (Tcl_InitStubs(interp, TCL_VERSION, NULL) == NULL) {
            return TCL_ERROR;
        }

        if (Tcl_PkgProvide(interp, "pyinter", "0.1") == TCL_ERROR) {
            return TCL_ERROR;
        }

        Tcl_CreateObjCommand(
            interp,
            "pyinter_import",
            Pyinter_Import,
            NULL,
            NULL);

        return TCL_OK;

    } catch (boost::python::error_already_set &e) {
        handle_exception();
        traceback();
        return TCL_ERROR;
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return TCL_ERROR;
    }
}