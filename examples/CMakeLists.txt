SET(CMAKE_CXX_FLAGS "-std=c++11")

add_executable(example1 example1.cpp) 
add_executable(reader reader.cpp)

target_link_libraries(example1 LINK_PUBLIC biobj_simplex)
target_link_libraries(reader LINK_PUBLIC biobj_simplex)
