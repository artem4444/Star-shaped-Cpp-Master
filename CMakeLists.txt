cmake_minimum_required(VERSION 3.14)
project(StarShapedCppMaster)

#Cpp language standards during compilation: version, strictness
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(SOURCES
    src/data_structuring.cpp
    src/Star_Manager.cpp
)

include_directories(include)

set(HEADERS
    include/data_structuring.hpp
    include/slaves_state_struct.hpp
    include/Star_Manager.hpp
)


# library=compiled code that other programs can link to and use
add_library(data_structuring_lib ${SOURCES} ${HEADERS})


enable_testing()

# folders will local cmakelists:
add_subdirectory(tests)

