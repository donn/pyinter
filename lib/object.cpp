#include "object.hpp"

#include "python.hpp"

#include <iostream>
#include <sstream>

using namespace boost::python;
using bpo = boost::python::object;

inline const char *describe(bpo &obj) {
    return extract< const char * >(str(obj));
}

inline const char *pyname(bpo &obj) {
    bpo nameAttribute = obj.attr("__name__");
    return extract< const char * >(nameAttribute);
}

bool pyint::object::callable() {
    return (bool)PyCallable_Check(ptr());
}

void pyint::object::processCallable() {
    dict dictionary = extract< dict >(attr("__dict__"));
    bpo resultRef = dictionary.get("__pyinter_annotation__");

    if (resultRef.is_none()) {
        return;
    }

    bool result = extract< bool >(resultRef);
    if (!result) {
        return;
    }

    auto annotationsOptional = getAnnotations();
    if (!annotationsOptional.has_value()) {
        std::stringstream ss;
        ss << "No type annotations found for pyinter annotated function \"" << pyname(*this) << "\"." << std::endl;

        throw std::runtime_error(ss.str());
    }

    auto annotations = annotationsOptional.value();

    for (auto &annotation : annotations) {
        auto &name = annotation.first;
        auto &value = annotation.second;

        if (name == "return") {
            continue;
        }

        arguments[name] = pyname(value);
    }
}

std::optional< std::map< std::string, boost::python::object > >
pyint::object::getAnnotations() {
    auto annotationsWeakRef = PyFunction_GetAnnotations(ptr());
    if (annotationsWeakRef == nullptr) {
        return std::nullopt;
    }
    auto annotationsHandle = handle<>(annotationsWeakRef);
    bpo annotationsRef(annotationsHandle);
    if (annotationsRef.is_none()) {
        return std::nullopt;
    }

    tuple annotations = extract< tuple >(annotationsRef);

    auto length = len(annotations);

    auto retval = std::map< std::string, boost::python::object >();

    for (auto i = 0; i < (length / 2); i += 1) {
        const char *key = extract< const char * >(annotations[2 * i]);
        auto value = annotations[2 * i + 1];

        retval[key] = value;
    }

    return retval;
}