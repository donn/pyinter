# 🐍🪶 Pyinter
[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0) [![C++ 11](https://img.shields.io/badge/c%2B%2B-11-lightgrey)](#) <!-- [![CI](https://github.com/donn/pyinter/workflows/CI/badge.svg?branch=main)](#) -->

Pyinter is a thin Tcl interoperability layer that allows the importing and usage of Python functions.

```Tcl
% load ./pyinter.so
% pyinter_import sys
% puts [sys::version]
3.10.1 (main, Dec 18 2021, 23:53:45) [GCC 11.1.0]
```

# Features
Pyinter can generate an OpenROAD/OpenLane style CLI from annotated Python functions.

This uses the Python type annotations to convert values from Tcl strings to Python variables.

```python
def math(a: int, b: int, add: bool = False) -> int:
    return a + b if add else a - b

math.__dict__["__pyinter_annotation__"] = True
```

```tcl
% math -a 2 -b 4 -add
6
% math -a 7 -b 2
5
```

# Current Limitations
* All return values, including those that return nontrivial objects, get a string description.
    * Meaning object-oriented programming is a no-go.
    * By extension, Pyinter only properly supports functions: methods cannot be called
* Type conversion only applies for keyword variables: positional variables only support strings

# Requirements
* C++11-compatible compiler
* Python 3.6+
* Tcl 8.5+
* Boost 1.69+
    * Boost::Python
    * Boost::Optional

# ⚖️ License
Copyright (c) 2022 Mohamed Gaber.

Available under thhe Apache License, version 2.0 ***with LLVM Exceptions***. See 'License'.