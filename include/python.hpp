#ifndef _python_hpp
#define _python_hpp

#include <boost/python.hpp>

// Various assisting functions for boost::python
namespace boost {
    namespace python {
        void traceback();
        bool callable(object obj);
    }
}

#endif