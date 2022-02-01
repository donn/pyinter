// Copyright 2022 Mohamed Gaber
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "object.hpp"

#include "python.hpp"

#include <iostream>
#include <queue>
#include <sstream>

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

    auto getdoc = inspect.attr("getdoc");
    auto pydoc = getdoc(target);

    if (pydoc.is_none()) {
        _documentation = "No documentation found for this function.";
    } else {
        auto documentation = describe(pydoc);
        _documentation = documentation;
    }
}

bool pyint::object::callable() const {
    return (bool)PyCallable_Check(ptr());
}

bool pyint::object::commandLineCallable() const {
    // Yes, this is how boost unwraps optionals. Leave me alone.
    if (fnSignature) {
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

    fnSignature = pyint::signature(*this);
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
    std::deque< std::string > arguments(
        std::deque< std::string >(rawArguments.begin(), rawArguments.end()));

    list argList;
    dict kwargs;

    bpo pytrue = eval("True");

    auto &parameters = fnSignature.value().parameters();

    auto pop = [&]() {
        if (arguments.size()) {
            auto retval = arguments.front();
            arguments.pop_front();
            return boost::optional<std::string>(retval);

        }
        return boost::optional<std::string>();
    };

    auto popValue = [&](std::string& key) {
        auto value = pop();
        if (value) {
            return value.value();
        }
        std::stringstream ss;

        ss << "raise ValueError('Non-boolean keyword argument \"" << key << "\" missing value.')" << std::endl;

        exec(ss.str().c_str());

        return std::string("The last exec should raise an exception and so this line should never be reached.");
    };

    while (!arguments.empty()) {
        auto argument = pop().value();

        if (argument[0] != '-') {
            argList.append(str(argument.c_str()));
            continue;
        }

        auto key = argument.substr(1);
        
        auto parameterIterator = parameters.find(key);

        // Check if parameter not found
        if (parameterIterator == parameters.end()) {
            // If the parameter is help and it's not found, just print the documentation.
            if (key == "help") {
                std::cerr << fnSignature.value().documentation() << std::endl;
                return std::string("");
            }

            // Let Python handle telling the user that the kwarg is not found.
            auto value = popValue(key);
            kwargs[key.c_str()] = str(value.c_str());
            continue;
        }

        auto& parameter = *parameterIterator;
        auto parameterTypeOptional = parameter.second.type();
        if (!parameterTypeOptional) {
            kwargs[key.c_str()] = str(argument.c_str());
        }

        auto parameterType = parameterTypeOptional.value();

        if (std::string(pyname(parameterType)) == std::string("bool")) {
            kwargs[key.c_str()] = pytrue;
        } else {
            auto value = popValue(key);
            kwargs[key.c_str()] = parameterType(str(value.c_str()));
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