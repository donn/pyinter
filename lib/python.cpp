#include "python.hpp"

void boost::python::traceback() {
    PyErr_Print();
}

bool boost::python::callable(boost::python::object obj) {
    return (bool)PyCallable_Check(obj.ptr());
}
