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