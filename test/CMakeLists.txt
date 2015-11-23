cmake_minimum_required(VERSION 2.8)
project(biobj-simplex-tests)
include_directories(../include
                    ../../mo-utils/include)
find_library(CPPUNIT_LIBRARY cppunit)
SET(CMAKE_CXX_FLAGS "-std=c++11")

set(TESTS_SRC main.cpp
              Biobj_simplex_test.cpp)

add_executable(biobj_simplex_tests ${TESTS_SRC})
target_link_libraries(biobj_simplex_tests LINK_PUBLIC biobj_simplex ${CPPUNIT_LIBRARY})
