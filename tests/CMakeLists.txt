#we separate tests cmakelist from the main one not to compile extra files in production stage

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

include_directories(${CMAKE_SOURCE_DIR}/include)


#defines the center of compilation around which other files will be linked
add_executable(test_data_structuring test_data_structuring.cpp)

target_link_libraries(test_data_structuring
    data_structuring_lib
    gtest
    gtest_main
)


#registering with CTest for standardization, CI/CD integration
enable_testing()
add_test(NAME DataStructuringTests COMMAND test_data_structuring)