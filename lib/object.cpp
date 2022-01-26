#include "object.hpp"

#include "python.hpp"

#include <iostream>
#include <sstream>

using namespace boost::python;
using bpo = boost::python::object;

pyint::signature::signature(bpo &target) {
    auto inspect = import("inspect");

    auto get_signature = inspect.attr("signature");

    auto Signature = inspect.attr("Signature");
    auto SignatureEmpty = Signature.attr("empty");

    auto Parameter = inspect.attr("Parameter");
    auto ParameterEmpty = Parameter.attr("empty");

    auto signature = get_signature(target);

    bpo returnAnnotation = signature.attr("return_annotation");

    if (returnAnnotation != SignatureEmpty) {
        _returnType = returnAnnotation;
    }

    dict parameterDict = dict(signature.attr("parameters"));
    list parameters = parameterDict.values();
    auto length = len(parameters);

    for (ssize_t i = 0; i < length; i += 1) {
        object parameter = parameters[i];

        const char *name = extract< const char * >(parameter.attr("name"));
        boost::optional< bpo > type = bpo(parameter.attr("annotation"));
        if (type.value() == ParameterEmpty) {
            type = boost::none;
        }
        boost::optional< bpo > defaultValue = bpo(parameter.attr("annotation"));
        if (defaultValue.value() == ParameterEmpty) {
            type = boost::none;
        }

        _parameters.push_back(pyint::parameter(name, type, defaultValue));
    }
}

bool pyint::object::callable() const {
    return (bool)PyCallable_Check(ptr());
}

void pyint::object::processCallable() {
    if (!boost::python::hasattr(*this, "__dict__")) {
        return;
    }
    
    dict dictionary = dict(attr("__dict__"));
    bpo resultRef = dictionary.get("__pyinter_annotation__");

    if (resultRef.is_none()) {
        return;
    }

    bool result = extract< bool >(resultRef);
    if (!result) {
        return;
    }

    auto signature = pyint::signature(*this);
}