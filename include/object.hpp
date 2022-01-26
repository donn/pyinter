#ifndef _object_hpp
#define _object_hpp

#include <boost/optional.hpp>
#include <boost/python.hpp>
#include <map>
#include <optional>

namespace pyint {
    class parameter {
        private:
            std::string _name;
            boost::optional< boost::python::object > _type;
            boost::optional< boost::python::object > _defaultValue;
        public:
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
            std::vector< parameter > _parameters;
        public:
            signature() {}
            signature(boost::python::object &target);

            boost::optional< boost::python::object > returnType() const {
                return _returnType;
            }
            const std::vector< parameter > &parameters() const {
                return _parameters;
            }
    };

    class object : public boost::python::object {
        private:
            void processCallable();
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
    };
}
#endif