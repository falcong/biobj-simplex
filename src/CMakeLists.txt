find_library(COIN_UTILS_LIBRARY CoinUtils)
find_library(OSI_CLP_LIBRARY OsiClp)

add_library(biobj_simplex biobj_simplex.cpp)
target_link_libraries(biobj_simplex ${COIN_UTILS_LIBRARY} ${OSI_CLP_LIBRARY})
