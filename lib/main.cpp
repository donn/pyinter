#include "python.hpp"
#include "util.hpp"

#include <boost/format.hpp>
#include <boost/python.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tcl.h>
#include <vector>

using boost::format;
boost::python::object moduleDictRef;

auto traceback = PyErr_Print;

extern "C" int RunPythonCommand(
    ClientData cdata,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const objv[]) {
    using namespace boost::python;

    try {
        object fn = moduleDictRef[const_cast< const char * >(objv[0]->bytes)];

        list arguments;
        for (int i = 1; i < objc; i += 1) {
            auto tclArg = objv[i];
            str arg = const_cast< const char * >(tclArg->bytes);
            arguments.append(arg);
        }
        tuple args = tuple(arguments);

        object result = fn(*args);

        std::string returnValue = "";
        if (!result.is_none()) {
            returnValue = extract< const char * >(result);
        }

        Tcl_SetObjResult(interp, Tcl_NewStringObj(returnValue.c_str(), -1));
        return TCL_OK;

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

function_list = getmembers(importable, isfunction)
)python3");

// Called when Tcl loads the extension
extern "C" int DLLEXPORT Pyinter_Init(Tcl_Interp *interp) {
    using namespace boost::python;
    try {
        Py_Initialize();

        auto importablePythonModule = util::env("PYINTER_MODULE");
        auto cwd = util::env("PWD");

        auto main = import("__main__");
        auto global = main.attr("__dict__");

        auto run = (initScriptFmt % cwd % importablePythonModule).str();

        exec(run.c_str(), global);

        auto topModule = global["importable"];
        auto functionListRef = global["function_list"];

        moduleDictRef = topModule.attr("__dict__");

        auto name =
            extract< const char * >(moduleDictRef["__TCL_PACKAGE_NAME__"]);
        auto version =
            extract< const char * >(moduleDictRef["__TCL_PACKAGE_VERSION__"]);

        if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
            return TCL_ERROR;
        }

        if (Tcl_PkgProvide(interp, name, version) == TCL_ERROR) {
            return TCL_ERROR;
        }

        list functions = extract< list >(functionListRef);

        for (ssize_t i = 0; i < len(functions); i += 1) {
            tuple function = extract< tuple >(functions[i]);
            auto name = extract< const char * >(function[0]);
            Tcl_CreateObjCommand(interp, name, RunPythonCommand, NULL, NULL);
        }
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