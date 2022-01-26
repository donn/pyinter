#include "python.hpp"

void boost::python::traceback() {
    PyErr_Print();
}
