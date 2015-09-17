find_library(COIN_UTILS_LIBRARY CoinUtils)
find_library(OSI_CLP_LIBRARY OsiClp)
SET(CMAKE_CXX_FLAGS "-std=c++11")
include_directories(../../mo-utils/include)

add_library(biobj_simplex biobj_simplex.cpp)
target_link_libraries(biobj_simplex ${COIN_UTILS_LIBRARY} ${OSI_CLP_LIBRARY})
