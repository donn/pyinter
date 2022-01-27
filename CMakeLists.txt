project(pyinter)
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0110 NEW)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

find_package(PythonLibs 3.6 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(TCL 8.6)
include_directories(${TCL_INCLUDE_PATH})

find_package(Boost 1.69 REQUIRED python)
include_directories(${Boost_INCLUDE_DIR})

add_library(pyinter SHARED lib/main.cpp lib/object.cpp lib/util.cpp)
target_link_libraries(pyinter LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(pyinter LINK_PUBLIC ${PYTHON_LIBRARY})

enable_testing()
add_subdirectory(test)