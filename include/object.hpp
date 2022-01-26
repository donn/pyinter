#ifndef _object_hpp
#define _object_hpp

#include <boost/python.hpp>
#include <map>
#include <optional>

namespace pyint {
    class object : public boost::python::object {
            void processCallable();
            std::optional< std::map< std::string, boost::python::object > >
            getAnnotations();

            std::map<std::string, std::string> arguments;
        public:
            // Constructs None
            object() : boost::python::object() {}

            template < class T > object(T const &x) : boost::python::object(x) {
                if (!std::is_same_v< T, boost::python::object >) {
                    return;
                }
                if (!callable()) {
                    return;
                }

                processCallable();
            }

            BOOST_PYTHON_DECL explicit object(boost::python::handle<> const &x)
                : boost::python::object(x) {}

            bool callable();
            bool tclCallable() { return arguments.size() > 0; }
            std::string parseTclFlags(std::vector<std::string> rawArguments);
    };
}
#endif