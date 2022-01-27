#include "object.hpp"

#include "python.hpp"

#include <iostream>
#include <sstream>
#include <queue>

using namespace boost::python;
using bpo = boost::python::object;

pyint::signature::signature(bpo &target) {
    auto inspect = import("inspect");

    auto getSignature = inspect.attr("signature");

    auto Signature = inspect.attr("Signature");
    auto SignatureEmpty = Signature.attr("empty");

    auto Parameter = inspect.attr("Parameter");
    auto ParameterEmpty = Parameter.attr("empty");

    auto signature = getSignature(target);

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

        _parameters[name] = pyint::parameter(name, type, defaultValue);
    }
}

bool pyint::object::callable() const {
    return (bool)PyCallable_Check(ptr());
}

bool pyint::object::commandLineCallable() const {
    // Yes, this is how boost unwraps optionals. Leave me alone.
    if (signature) {
        return true;
    }
    return false;
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

    signature = pyint::signature(*this);
}

std::string pyint::object::call(std::vector< std::string > &stringArguments) {
    list argList;
    for (auto &stringArgument : stringArguments) {
        argList.append(str(stringArgument.c_str()));
    }
    tuple args = tuple(argList);

    auto result = (*this)(*args);

    auto returnValue = std::string("");
    if (!result.is_none()) {
        returnValue = extract< const char * >(str(result));
    }
    return returnValue;
}

std::string pyint::object::cli(std::vector< std::string > &rawArguments) {
    std::deque<std::string> arguments(std::deque<std::string>(rawArguments.begin(), rawArguments.end()));

    list argList;
    dict kwargs;

    bpo pybool = eval("bool");
    bpo pytrue = eval("True");

    auto& parameters = signature.value().parameters();
    while (!arguments.empty()) {
        auto argument = arguments.front();
        arguments.pop_front();

        if (argument[0] != '-') {
            argList.append(str(argument.c_str()));
        } else {
            auto key = argument.substr(1);
            auto parameter = parameters[key];

            auto parameterTypeOptional = parameter.type();
            if (!parameterTypeOptional) {
                kwargs[key.c_str()] = str(argument.c_str());
            }

            auto parameterType = parameterTypeOptional.value();

            if (parameterType == pybool) {
                kwargs[key.c_str()] = pytrue;
            } else {
                auto value = arguments.front();
                arguments.pop_front();

                kwargs[key.c_str()] = parameterType(str(value.c_str()));
            }
        }
    }

    tuple args = tuple(argList);

    auto result = (*this)(*args, **kwargs);

    std::cerr << describe(result) << std::endl;

    auto returnValue = std::string("");
    if (!result.is_none()) {
        returnValue = extract< const char * >(str(result));
    }

    return returnValue;
}