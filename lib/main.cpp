#include "util.hpp"

#include <boost/format.hpp>
#include <boost/python.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tcl.h>
#include <unordered_map>
#include <vector>

using boost::format;

std::vector< std::shared_ptr< boost::python::object > > objectStore;

namespace boost {
    namespace python {
        auto traceback = PyErr_Print;
        bool callable(object obj) {
            return (bool)PyCallable_Check(obj.ptr());
        }
    }
}


extern "C" int RunPythonCommand(
    ClientData cdata,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const objv[]) {
    using namespace boost::python;

    try {
        object fn = *static_cast< object * >(cdata);

        if (callable(fn)) {
            list arguments;
            for (int i = 1; i < objc; i += 1) {
                auto tclArg = objv[i];
                str arg = const_cast< const char * >(tclArg->bytes);
                arguments.append(arg);
            }
            tuple args = tuple(arguments);

            object result = fn(*args);

            auto returnValue = std::string("");
            if (!result.is_none()) {
                returnValue = extract< const char * >(result);
            }

            Tcl_SetObjResult(interp, Tcl_NewStringObj(returnValue.c_str(), -1));
            return TCL_OK;

        } else {
            auto returnValue = extract< const char * >(str(fn));
            Tcl_SetObjResult(interp, Tcl_NewStringObj(returnValue, -1));
            return TCL_OK;
        }

    } catch (boost::python::error_already_set &e) {
        handle_exception();
        traceback();
        return TCL_ERROR;
    }
}

auto initScriptFmt = format(R"python3(
import sys
from inspect import getmembers, isfunction

sys.path.append("%s")

import %s as importable

members = getmembers(importable)
)python3");

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

        auto cwd = util::env("PWD");
        auto importablePythonModule = std::string(objv[1]->bytes);

        auto main = import("__main__");
        auto global = main.attr("__dict__");

        auto run = (initScriptFmt % cwd % importablePythonModule).str();

        exec(run.c_str(), global);

        auto topModule = global["importable"];

        auto ns = Tcl_CreateNamespace(
            interp,
            importablePythonModule.c_str(),
            NULL,
            NULL);
        Tcl_Export(interp, ns, "*", 0);

        list members = extract< list >(global["members"]);
        for (ssize_t i = 0; i < len(members); i += 1) {
            tuple member = extract< tuple >(members[i]);
            const char *name = extract< const char * >(member[0]);
            std::string namespaced = importablePythonModule + "::" + name;

            object objectRef = extract< object >(member[1]);
            auto pointer = std::make_shared< object >(objectRef);

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
    } catch (boost::python::error_already_set &e) {
        handle_exception();
        traceback();
        return TCL_ERROR;
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return TCL_ERROR;
    }
    return TCL_OK;
}